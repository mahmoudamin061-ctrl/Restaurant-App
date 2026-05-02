#include "Restaurant.h"
#include "Action.h"
#include "UI.h"
#include <iostream>
#include <fstream>
#include <string>
#include "RequestAction.h"
#include "CancelAction.h"

using namespace std;

// ─────────────────────────────────────────────────────────────────────────────
// Constructor / Destructor
// ─────────────────────────────────────────────────────────────────────────────
Restaurant::Restaurant()
    : currentTimeStep(0), TH(0),
    totalCNBusyTime(0), totalCSBusyTime(0),
    totalCNCount(0), totalCSCount(0),
    totalScooterBusyTime(0), totalScooterCount(0),
    countODG(0), countODN(0), countOT(0),
    countOVG(0), countOVC(0), countOVN(0)
{
}

Restaurant::~Restaurant() {}

// ─────────────────────────────────────────────────────────────────────────────
// AddAction / AddOrder
// ─────────────────────────────────────────────────────────────────────────────
void Restaurant::AddAction(Action* pAct) {
    if (pAct) ACTIONS_LIST.enqueue(pAct);
}

void Restaurant::AddOrder(Order* pOrd) {
    if (!pOrd) return;
    switch (pOrd->getType()) {
    case ODG: PEND_ODG.enqueue(pOrd);                           countODG++; break;
    case ODN: PEND_ODN.enqueue(pOrd);                           countODN++; break;
    case OT:  PEND_OT.enqueue(pOrd);                            countOT++;  break;
    case OVG: PEND_OVG.enqueue(pOrd, pOrd->getPriority());      countOVG++; break;
    case OVC: PEND_OVC.enqueue(pOrd);                           countOVC++; break;
    case OVN: PEND_OVN.enqueue(pOrd);                           countOVN++; break;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// FIX #1 – Cancel only OVC orders (spec: "Order cancellation for OVC orders only")
//   Searches pending OVC, ready OVC, and cooking list (OVC check inside).
// ─────────────────────────────────────────────────────────────────────────────
static bool CancelFromQueue(LinkedQueue<Order*>& q, int id,
    LinkedQueue<Order*>& cancelledList) {
    LinkedQueue<Order*> tmp;
    Order* o;
    bool found = false;
    while (q.dequeue(o)) {
        if (!found && o->getID() == id) { cancelledList.enqueue(o); found = true; }
        else tmp.enqueue(o);
    }
    while (tmp.dequeue(o)) q.enqueue(o);
    return found;
}

bool Restaurant::CancelFromCookingOVC(int id) {
    // Only cancel if it's an OVC order that is currently cooking
    LinkedQueue<CookingEntry*> tmp;
    CookingEntry* e;
    bool found = false;
    while (COOKING.dequeue(e)) {
        if (!found && e->order->getID() == id && e->order->getType() == OVC) {
            // Release chef back to correct pool
            if (e->chef->getType() == CN) availableCN.enqueue(e->chef);
            else                           availableCS.enqueue(e->chef);
            CANCELLED.enqueue(e->order);
            delete e;
            found = true;
        }
        else {
            tmp.enqueue(e);
        }
    }
    while (tmp.dequeue(e)) COOKING.enqueue(e);
    return found;
}

bool Restaurant::RemoveOrderOVC(int id) {
    // Check pending OVC
    if (CancelFromQueue(PEND_OVC, id, CANCELLED)) return true;
    // Check ready OVC
    if (CancelFromQueue(READY_OVC, id, CANCELLED)) return true;
    // Check cooking list (OVC only)
    if (CancelFromCookingOVC(id)) return true;
    // Check in-service (OVC order already assigned to scooter)
    {
        LinkedQueue<InServiceEntry*> tmp;
        InServiceEntry* e;
        bool found = false;
        while (INSERVICE_LIST.dequeue(e)) {
            if (!found && e->order->getID() == id && e->order->getType() == OVC) {
                // Release scooter back
                returningScooters.enqueue(e->scooter);
                CANCELLED.enqueue(e->order);
                delete e;
                found = true;
            }
            else {
                tmp.enqueue(e);
            }
        }
        while (tmp.dequeue(e)) INSERVICE_LIST.enqueue(e);
        if (found) return true;
    }
    return false;
}

// ─────────────────────────────────────────────────────────────────────────────
// FIX #2 – Chef retrieval with correct fallback rules
//   preferred = the primary type wanted
//   allowFallback = true means try the other type if preferred unavailable
// ─────────────────────────────────────────────────────────────────────────────
Chef* Restaurant::GetFreeChef(CHEF_TYPE preferred, bool allowFallback) {
    Chef* c = nullptr;
    LinkedQueue<Chef*>& primaryPool = (preferred == CN) ? availableCN : availableCS;
    LinkedQueue<Chef*>& fallbackPool = (preferred == CN) ? availableCS : availableCN;

    if (primaryPool.dequeue(c)) return c;
    if (allowFallback && fallbackPool.dequeue(c)) return c;
    return nullptr;
}

// ─────────────────────────────────────────────────────────────────────────────
// Assign a chef to an order and push to COOKING
// ─────────────────────────────────────────────────────────────────────────────
void Restaurant::AssignChefToOrder(Order* ord, Chef* chef) {
    ord->setTA(currentTimeStep);
    chef->assignOrder(ord->getID());
    CookingEntry* entry = new CookingEntry();
    entry->order = ord;
    entry->chef = chef;
    // ceiling division: number of timesteps to finish cooking
    entry->remainingTime = (ord->getSize() + chef->getSpeed() - 1) / chef->getSpeed();
    COOKING.enqueue(entry);
}

// ─────────────────────────────────────────────────────────────────────────────
// FIX #3 – Full chef-assignment priority order per spec:
//   1) ODG  → CS only
//   2) ODN  → CN preferred, CS fallback
//   3) OT   → CN only (no fallback)
//   4) OVG  → CS only
//   5) OVC  → CN preferred, CS fallback
//   6) OVN  → CN only (no fallback)
// ─────────────────────────────────────────────────────────────────────────────
void Restaurant::AssignPendingToChefs() {
    Order* ord; int pri; Chef* chef;

    // 1) ODG → CS only
    while (PEND_ODG.peek(ord)) {
        chef = GetFreeChef(CS, false);
        if (!chef) break;
        PEND_ODG.dequeue(ord);
        AssignChefToOrder(ord, chef);
    }

    // 2) ODN → CN preferred, CS fallback
    while (PEND_ODN.peek(ord)) {
        chef = GetFreeChef(CN, true);
        if (!chef) break;
        PEND_ODN.dequeue(ord);
        AssignChefToOrder(ord, chef);
    }

    // 3) OT → CN only
    while (PEND_OT.peek(ord)) {
        chef = GetFreeChef(CN, false);
        if (!chef) break;
        PEND_OT.dequeue(ord);
        AssignChefToOrder(ord, chef);
    }

    // 4) OVG → CS only (priority queue)
    while (PEND_OVG.peek(ord, pri)) {
        chef = GetFreeChef(CS, false);
        if (!chef) break;
        PEND_OVG.dequeue(ord, pri);
        AssignChefToOrder(ord, chef);
    }

    // 5) OVC → CN preferred, CS fallback
    while (PEND_OVC.peek(ord)) {
        chef = GetFreeChef(CN, true);
        if (!chef) break;
        PEND_OVC.dequeue(ord);
        AssignChefToOrder(ord, chef);
    }

    // 6) OVN → CN only
    while (PEND_OVN.peek(ord)) {
        chef = GetFreeChef(CN, false);
        if (!chef) break;
        PEND_OVN.dequeue(ord);
        AssignChefToOrder(ord, chef);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Advance cooking: decrement remaining time, move finished orders to ready queues
// ─────────────────────────────────────────────────────────────────────────────
void Restaurant::AdvanceCooking() {
    LinkedQueue<CookingEntry*> stillCooking;
    CookingEntry* entry;

    while (COOKING.dequeue(entry)) {
        entry->remainingTime--;

        if (entry->remainingTime <= 0) {
            Order* ord = entry->order;
            Chef* chef = entry->chef;
            ord->setTR(currentTimeStep);

            // Accumulate chef busy time for utilization stats
            if (chef->getType() == CN) totalCNBusyTime += ord->getCookTime();
            else                       totalCSBusyTime += ord->getCookTime();

            chef->releaseOrder();
            // Return chef to correct pool
            if (chef->getType() == CN) availableCN.enqueue(chef);
            else                       availableCS.enqueue(chef);
            delete entry;

            switch (ord->getType()) {
            case ODG: READY_ODG.enqueue(ord); break;
            case ODN: READY_ODN.enqueue(ord); break;
            case OT:  READY_OT.enqueue(ord);  break;
            case OVG: READY_OVG.enqueue(ord); break;
            case OVC: READY_OVC.enqueue(ord); break;
            case OVN: READY_OVN.enqueue(ord); break;
            }
        }
        else {
            stillCooking.enqueue(entry);
        }
    }
    while (stillCooking.dequeue(entry)) COOKING.enqueue(entry);
}

// ─────────────────────────────────────────────────────────────────────────────
// FIX #4 – Overwait check for ready OVG orders
//   If currentTime - TR > TH, move to overwait priority queue
//   Priority in overwait list = currentTime - TQ (higher = served first)
// ─────────────────────────────────────────────────────────────────────────────
void Restaurant::CheckOverwait() {
    LinkedQueue<Order*> stillReady;
    Order* ord;
    while (READY_OVG.dequeue(ord)) {
        if (currentTimeStep - ord->getTR() > TH) {
            int waitPri = currentTimeStep - ord->getTQ();
            READY_OVG_OVERWAIT.enqueue(ord, waitPri);
        }
        else {
            stillReady.enqueue(ord);
        }
    }
    while (stillReady.dequeue(ord)) READY_OVG.enqueue(ord);
}

// ─────────────────────────────────────────────────────────────────────────────
// FIX #5 – Table best-fit with sharing support
//   sharing = true: look for a table that already has occupants first (can share),
//             then fall back to any table with enough free seats
// ─────────────────────────────────────────────────────────────────────────────
Table* Restaurant::FindBestFitTable(int seats, bool sharingOk) {
    // We scan availableTables queue.
    // "Available" means the table exists in the list; freeSeats may be < capacity
    // if sharing is active (we keep partially-occupied shareable tables in the list).

    LinkedQueue<Table*> tmp;
    Table* t;
    Table* bestShared = nullptr;
    Table* bestFresh = nullptr;

    while (availableTables.dequeue(t)) {
        if (t->getFreeSeats() >= seats) {
            bool isPartial = t->getFreeSeats() < t->getCapacity();
            if (isPartial && sharingOk) {
                if (!bestShared || t->getFreeSeats() < bestShared->getFreeSeats())
                    bestShared = t;
            }
            else if (!isPartial) {
                if (!bestFresh || t->getFreeSeats() < bestFresh->getFreeSeats())
                    bestFresh = t;
            }
        }
        tmp.enqueue(t);
    }

    Table* chosen = bestShared ? bestShared : bestFresh;
    // Restore all tables EXCEPT chosen (remove it from the queue)
    while (tmp.dequeue(t)) {
        if (t != chosen) availableTables.enqueue(t);
    }
    return chosen;
}

// ─────────────────────────────────────────────────────────────────────────────
// Scooter: shortest total distance so far assigned first
// ─────────────────────────────────────────────────────────────────────────────
Scooter* Restaurant::FindShortestDistanceScooter() {
    LinkedQueue<Scooter*> tmp;
    Scooter* s; Scooter* best = nullptr;
    while (availableScooters.dequeue(s)) {
        if (!best || s->getTotalDistance() < best->getTotalDistance()) best = s;
        tmp.enqueue(s);
    }
    // Restore all EXCEPT the chosen scooter (remove it from available pool)
    while (tmp.dequeue(s)) {
        if (s != best) availableScooters.enqueue(s);
    }
    return best;
}

// ─────────────────────────────────────────────────────────────────────────────
// Assign dine-in orders to tables (ODG and ODN)
// ─────────────────────────────────────────────────────────────────────────────
void Restaurant::AssignDineInOrders(LinkedQueue<Order*>& readyList) {
    LinkedQueue<Order*> waiting;
    Order* ord;
    while (readyList.dequeue(ord)) {
        // Try sharing table first, then any free table
        Table* best = FindBestFitTable(ord->getSeats(), ord->getCanShare());
        if (!best && ord->getCanShare())
            best = FindBestFitTable(ord->getSeats(), false); // no shared table found; try fresh

        if (best) {
            ord->setTS(currentTimeStep);
            best->occupy(ord->getSeats(), ord->getDuration(), currentTimeStep);
            // If table still has free seats after this order, put it back (sharing)
            if (best->getFreeSeats() > 0)
                availableTables.enqueue(best);
            InServiceEntry* se = new InServiceEntry();
            se->order = ord;
            se->scooter = nullptr;
            se->table = best;
            INSERVICE_LIST.enqueue(se);
        }
        else {
            waiting.enqueue(ord);
        }
    }
    while (waiting.dequeue(ord)) readyList.enqueue(ord);
}

// ─────────────────────────────────────────────────────────────────────────────
// FIX #6 – Delivery assignment: OVC first, then any other OV (FCFS within type)
//           Uses overwait priority queue before normal OVG queue
// ─────────────────────────────────────────────────────────────────────────────
void Restaurant::AssignDeliveryBatch() {
    auto assignOne = [&](Order* ord) -> bool {
        Scooter* s = FindShortestDistanceScooter();
        if (!s) return false;
        ord->setTS(currentTimeStep);
        s->addDistance(ord->getDistance()); // one-way; return counted in TF calc
        s->recordDelivery();
        InServiceEntry* se = new InServiceEntry();
        se->order = ord;
        se->scooter = s;
        se->table = nullptr;
        INSERVICE_LIST.enqueue(se);
        return true;
        };

    // 1) Overwait OVG (bonus – highest priority)
    {
        Order* ord; int pri;
        LinkedQueue<Order*> waiting;
        while (READY_OVG_OVERWAIT.peek(ord, pri)) {
            READY_OVG_OVERWAIT.dequeue(ord, pri);
            if (!assignOne(ord)) { waiting.enqueue(ord); break; }
        }
        while (waiting.dequeue(ord)) READY_OVG_OVERWAIT.enqueue(ord, currentTimeStep - ord->getTQ());
    }

    // 2) OVC orders first
    {
        LinkedQueue<Order*> waiting; Order* ord;
        while (READY_OVC.dequeue(ord)) {
            if (!assignOne(ord)) { waiting.enqueue(ord); break; }
        }
        while (waiting.dequeue(ord)) READY_OVC.enqueue(ord);
    }

    // 3) Then normal OVG
    {
        Order* ord; int pri;
        LinkedQueue<Order*> waiting;
        while (READY_OVG.dequeue(ord)) {
            if (!assignOne(ord)) { waiting.enqueue(ord); break; }
        }
        while (waiting.dequeue(ord)) READY_OVG.enqueue(ord);
    }

    // 4) Then OVN
    {
        LinkedQueue<Order*> waiting; Order* ord;
        while (READY_OVN.dequeue(ord)) {
            if (!assignOne(ord)) { waiting.enqueue(ord); break; }
        }
        while (waiting.dequeue(ord)) READY_OVN.enqueue(ord);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// FIX #7 – OT: wait exactly 1 timestep after cooking (packing time)
//   TS is set when order first seen in READY_OT.
//   TF is set one timestep later.
// ─────────────────────────────────────────────────────────────────────────────
void Restaurant::AssignReadyOrders() {
    // --- Takeaway ---
    {
        LinkedQueue<Order*> stillWaiting; Order* ord;
        while (READY_OT.dequeue(ord)) {
            if (ord->getTS() == 0) {
                ord->setTS(currentTimeStep); // start packing now
                stillWaiting.enqueue(ord);
            }
            else {
                // already packed for 1 step → customer picks up
                ord->setTF(currentTimeStep);
                FINISHED.enqueue(ord);
            }
        }
        while (stillWaiting.dequeue(ord)) READY_OT.enqueue(ord);
    }

    // --- Dine-in ---
    AssignDineInOrders(READY_ODG);
    AssignDineInOrders(READY_ODN);

    // --- Overwait check before delivery ---
    CheckOverwait();

    // --- Delivery (OVC first, then OVG, then OVN) ---
    AssignDeliveryBatch();
}

// ─────────────────────────────────────────────────────────────────────────────
// FIX #8 – Delivery finish time: one-way travel only (distance / speed)
//   The spec says TF = TS + distance/speed (delivery).
//   Previously the code doubled the distance by mistake.
// ─────────────────────────────────────────────────────────────────────────────
void Restaurant::FinishInServiceOrders() {
    LinkedQueue<InServiceEntry*> stillInService;
    InServiceEntry* entry;

    while (INSERVICE_LIST.dequeue(entry)) {
        Order* ord = entry->order;
        bool   done = false;
        ORD_TYPE t = ord->getType();

        if (t == OVG || t == OVC || t == OVN) {
            // TF = TS + ceil(distance / speed)
            int travelTime = (ord->getDistance() + entry->scooter->getSpeed() - 1)
                / entry->scooter->getSpeed();
            // DEBUG: remove before submission
            std::cout << "[DBG] OrdID=" << ord->getID()
                << " TS=" << ord->getTS()
                << " dist=" << ord->getDistance()
                << " spd=" << entry->scooter->getSpeed()
                << " travelTime=" << travelTime
                << " current=" << currentTimeStep
                << " finishAt=" << (ord->getTS() + travelTime) << "\n";
            if (currentTimeStep >= ord->getTS() + travelTime) {
                ord->setTF(currentTimeStep);
                FINISHED.enqueue(ord);
                totalScooterBusyTime += travelTime;
                returningScooters.enqueue(entry->scooter);
                delete entry;
                done = true;
            }
        }
        else {
            // OD: TF = TS + duration
            if (currentTimeStep >= ord->getTS() + ord->getDuration()) {
                ord->setTF(currentTimeStep);
                FINISHED.enqueue(ord);
                entry->table->freeTable();
                availableTables.enqueue(entry->table);
                delete entry;
                done = true;
            }
        }
        if (!done) stillInService.enqueue(entry);
    }
    while (stillInService.dequeue(entry)) INSERVICE_LIST.enqueue(entry);

    // Process returning scooters → maintenance or available
    Scooter* s;
    while (returningScooters.dequeue(s)) {
        if (s->needsMaintenance()) {
            s->startMaintenance(currentTimeStep);  // records finish time
            maintenanceScooters.enqueue(s);
        }
        else {
            availableScooters.enqueue(s);
        }
    }

    // Release scooters whose maintenance period has ended
    LinkedQueue<Scooter*> stillMaintaining;
    while (maintenanceScooters.dequeue(s)) {
        if (s->maintenanceDone(currentTimeStep)) {
            s->finishMaintenance();
            availableScooters.enqueue(s);
        }
        else {
            stillMaintaining.enqueue(s);
        }
    }
    while (stillMaintaining.dequeue(s)) maintenanceScooters.enqueue(s);
}

// ─────────────────────────────────────────────────────────────────────────────
// Simulation done when all queues except FINISHED/CANCELLED are empty
// ─────────────────────────────────────────────────────────────────────────────
bool Restaurant::IsSimulationDone() const {
    return ACTIONS_LIST.isEmpty()
        && PEND_ODG.isEmpty() && PEND_ODN.isEmpty()
        && PEND_OT.isEmpty() && PEND_OVG.isEmpty()
        && PEND_OVC.isEmpty() && PEND_OVN.isEmpty()
        && COOKING.isEmpty()
        && READY_ODG.isEmpty() && READY_ODN.isEmpty()
        && READY_OT.isEmpty() && READY_OVG.isEmpty()
        && READY_OVC.isEmpty() && READY_OVN.isEmpty()
        && READY_OVG_OVERWAIT.isEmpty()
        && INSERVICE_LIST.isEmpty();
}

// ─────────────────────────────────────────────────────────────────────────────
// Execute all actions whose ActionTime == currentTime
// ─────────────────────────────────────────────────────────────────────────────
void Restaurant::ExecuteEvents(int currentTime) {
    Action* pAct = nullptr;
    while (ACTIONS_LIST.peek(pAct)) {
        if (pAct->getActionTime() == currentTime) {
            ACTIONS_LIST.dequeue(pAct);
            pAct->Execute();
            delete pAct;
        }
        else break;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// LoadFromFile – reads the input file exactly as the spec describes
// ─────────────────────────────────────────────────────────────────────────────
void Restaurant::LoadFromFile(const string& filename) {
    ifstream fin(filename);
    if (!fin) { cout << "Error opening input file: " << filename << endl; return; }

    int cnCount, csCount;   fin >> cnCount >> csCount;
    int cnSpeed, csSpeed;   fin >> cnSpeed >> csSpeed;
    int sCount, sSpeed;     fin >> sCount >> sSpeed;
    int mainOrds, mainDur;  fin >> mainOrds >> mainDur;
    int totalTables;        fin >> totalTables;

    // Tables: pairs of (count, capacity) summing to totalTables
    int tableID = 0, tablesRead = 0;
    while (tablesRead < totalTables) {
        int tCount, tCap; fin >> tCount >> tCap;
        for (int i = 0; i < tCount; i++)
            availableTables.enqueue(new Table(++tableID, tCap));
        tablesRead += tCount;
    }

    fin >> TH; // overwait threshold

    // Build chef pools
    totalCNCount = cnCount;
    totalCSCount = csCount;
    int chefID = 1;
    for (int i = 0; i < cnCount; i++) availableCN.enqueue(new Chef(chefID++, CN, cnSpeed));
    for (int i = 0; i < csCount; i++) availableCS.enqueue(new Chef(chefID++, CS, csSpeed));

    // Build scooter pool
    totalScooterCount = sCount;
    for (int i = 1; i <= sCount; i++)
        availableScooters.enqueue(new Scooter(i, sSpeed, mainOrds, mainDur));

    int M; fin >> M;

    for (int i = 0; i < M; i++) {
        char actionType; fin >> actionType;

        if (actionType == 'Q') {
            string typStr; int TQ, ID, size; double price;
            fin >> typStr >> TQ >> ID >> size >> price;

            ORD_TYPE ot;
            if (typStr == "ODG") ot = ODG;
            else if (typStr == "ODN") ot = ODN;
            else if (typStr == "OT")  ot = OT;
            else if (typStr == "OVG") ot = OVG;
            else if (typStr == "OVC") ot = OVC;
            else                      ot = OVN;

            Order* ord = new Order(ID, ot, TQ);
            ord->setSize(size);
            ord->setPrice(price);

            if (ot == ODG || ot == ODN) {
                int seats, duration; char shareChar;
                fin >> seats >> duration >> shareChar;
                ord->setSeats(seats);
                ord->setDuration(duration);
                ord->setCanShare(shareChar == 'Y');
            }
            if (ot == OVG || ot == OVC || ot == OVN) {
                int distance; fin >> distance;
                ord->setDistance(distance);
            }

            AddAction(new RequestAction(TQ, this, ord));
        }
        else if (actionType == 'X') {
            int Tcancel, ID; fin >> Tcancel >> ID;
            AddAction(new CancelAction(Tcancel, this, ID));
        }
    }
    fin.close();
}

// ─────────────────────────────────────────────────────────────────────────────
// FIX #9 – SaveToFile: full statistics as required by spec
// ─────────────────────────────────────────────────────────────────────────────
void Restaurant::SaveToFile(const string& filename) {
    ofstream fout(filename);
    if (!fout) { cout << "Error opening output file: " << filename << endl; return; }

    // Collect all finished orders into a temporary vector-like structure
    // using a second queue, then sort by TF descending with selection sort
    LinkedQueue<Order*> temp;
    Order* ord;
    while (FINISHED.dequeue(ord)) temp.enqueue(ord);

    // Write output lines sorted by TF descending
    // We repeatedly find the max-TF order and write it
    LinkedQueue<Order*> remaining;
    while (temp.dequeue(ord)) remaining.enqueue(ord);

    // Compute averages while printing
    double sumTi = 0, sumTc = 0, sumTw = 0, sumTserv = 0;
    int finCount = remaining.getCount();

    // Build sorted output using a simple O(n^2) selection sort on queue
    // (acceptable for project scale; no STL allowed)
    for (int written = 0; written < finCount; written++) {
        LinkedQueue<Order*> scan;
        Order* maxOrd = nullptr;
        while (remaining.dequeue(ord)) {
            if (!maxOrd || ord->getTF() > maxOrd->getTF()) maxOrd = ord;
            scan.enqueue(ord);
        }
        fout << maxOrd->getTF() << "\t"
            << maxOrd->getID() << "\t"
            << maxOrd->getTQ() << "\t"
            << maxOrd->getTA() << "\t"
            << maxOrd->getTR() << "\t"
            << maxOrd->getTS() << "\t"
            << maxOrd->getIdleTime() << "\t"
            << maxOrd->getCookTime() << "\t"
            << maxOrd->getWaitTime() << "\t"
            << maxOrd->getServiceTime() << "\n";

        sumTi += maxOrd->getIdleTime();
        sumTc += maxOrd->getCookTime();
        sumTw += maxOrd->getWaitTime();
        sumTserv += maxOrd->getServiceTime();

        FINISHED.enqueue(maxOrd);
        while (scan.dequeue(ord)) if (ord != maxOrd) remaining.enqueue(ord);
    }

    int cancelCount = CANCELLED.getCount();
    int total = finCount + cancelCount;

    // Overwait count: count orders in overwait queue
    int overwaitCount = READY_OVG_OVERWAIT.getCount();

    fout << "\n========== STATISTICS ==========\n";

    // 1) Total orders and per-type counts
    fout << "1) Total orders: " << total << "\n"
        << "   ODG: " << countODG << "  ODN: " << countODN
        << "  OT: " << countOT << "  OVG: " << countOVG
        << "  OVC: " << countOVC << "  OVN: " << countOVN << "\n";

    // 2) Chefs
    fout << "2) Total chefs: " << (totalCNCount + totalCSCount)
        << "   CN: " << totalCNCount << "  CS: " << totalCSCount << "\n";

    // 3) Scooters
    fout << "3) Total scooters: " << totalScooterCount << "\n";

    // 4) Finished / cancelled percentages
    double finPct = total ? 100.0 * finCount / total : 0;
    double cancelPct = total ? 100.0 * cancelCount / total : 0;
    fout << "4) Finished: " << finCount << " (" << finPct << "%)"
        << "   Cancelled: " << cancelCount << " (" << cancelPct << "%)\n";

    // 5) Overwait percentage (of finished)
    double owPct = finCount ? 100.0 * overwaitCount / finCount : 0;
    fout << "5) Overwait orders: " << overwaitCount << " (" << owPct << "% of finished)\n";

    // 6) Averages
    if (finCount > 0) {
        fout << "6) Averages (finished orders):\n"
            << "   Avg Ti    = " << sumTi / finCount << "\n"
            << "   Avg Tc    = " << sumTc / finCount << "\n"
            << "   Avg Tw    = " << sumTw / finCount << "\n"
            << "   Avg Tserv = " << sumTserv / finCount << "\n";
    }

    // 7) Scooter utilization: (total busy timesteps) / (scooterCount * currentTimeStep) * 100
    double scooterUtil = (totalScooterCount > 0 && currentTimeStep > 0)
        ? 100.0 * totalScooterBusyTime / (totalScooterCount * currentTimeStep) : 0;
    fout << "7) Scooter utilization: " << scooterUtil << "%\n";

    // 8) Chef utilization
    int totalChefs = totalCNCount + totalCSCount;
    int totalChefBusy = totalCNBusyTime + totalCSBusyTime;
    double chefUtil = (totalChefs > 0 && currentTimeStep > 0)
        ? 100.0 * totalChefBusy / (totalChefs * currentTimeStep) : 0;
    fout << "8) Chef utilization:    " << chefUtil << "%\n";

    fout.close();
}

// ─────────────────────────────────────────────────────────────────────────────
// Main simulation loop
// ─────────────────────────────────────────────────────────────────────────────
void Restaurant::RunSimulation(UI* ui) {
    // Mode is selected in main() before this is called
    while (true) {
        currentTimeStep++;
        ExecuteEvents(currentTimeStep);
        AssignPendingToChefs();
        AdvanceCooking();
        AssignReadyOrders();
        FinishInServiceOrders();

        if (ui->GetMode() == MODE_INTR)
            ui->PrintAll(this, currentTimeStep);

        if (IsSimulationDone()) break;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Accessors for UI
// ─────────────────────────────────────────────────────────────────────────────
int                           Restaurant::GetTimeStep()            const { return currentTimeStep; }
LinkedQueue<Action*>& Restaurant::GetActions() { return ACTIONS_LIST; }
LinkedQueue<Order*>& Restaurant::GetPendingODN() { return PEND_ODN; }
LinkedQueue<Order*>& Restaurant::GetPendingODG() { return PEND_ODG; }
priQueue<Order*>& Restaurant::GetPendingOVG() { return PEND_OVG; }
LinkedQueue<Order*>& Restaurant::GetPendingOT() { return PEND_OT; }
LinkedQueue<Order*>& Restaurant::GetPendingOVN() { return PEND_OVN; }
LinkedQueue<Order*>& Restaurant::GetPendingOVC() { return PEND_OVC; }
LinkedQueue<CookingEntry*>& Restaurant::GetCooking() { return COOKING; }
LinkedQueue<Order*>& Restaurant::GetReadyODN() { return READY_ODN; }
LinkedQueue<Order*>& Restaurant::GetReadyODG() { return READY_ODG; }
LinkedQueue<Order*>& Restaurant::GetReadyOT() { return READY_OT; }
LinkedQueue<Order*>& Restaurant::GetReadyOVN() { return READY_OVN; }
LinkedQueue<Order*>& Restaurant::GetReadyOVC() { return READY_OVC; }
LinkedQueue<Order*>& Restaurant::GetReadyOVG() { return READY_OVG; }
priQueue<Order*>& Restaurant::GetReadyOVGOverwait() { return READY_OVG_OVERWAIT; }
LinkedQueue<InServiceEntry*>& Restaurant::GetInService() { return INSERVICE_LIST; }
LinkedQueue<Order*>& Restaurant::GetFinished() { return FINISHED; }
LinkedQueue<Order*>& Restaurant::GetCancelled() { return CANCELLED; }
LinkedQueue<Chef*>& Restaurant::GetAvailableCN() { return availableCN; }
LinkedQueue<Chef*>& Restaurant::GetAvailableCS() { return availableCS; }
LinkedQueue<Scooter*>& Restaurant::GetAvailableScooters() { return availableScooters; }
LinkedQueue<Scooter*>& Restaurant::GetMaintenanceScooters() { return maintenanceScooters; }
LinkedQueue<Scooter*>& Restaurant::GetReturningScooters() { return returningScooters; }
LinkedQueue<Table*>& Restaurant::GetAvailableTables() { return availableTables; }
