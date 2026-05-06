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
// Constructor
// ─────────────────────────────────────────────────────────────────────────────
Restaurant::Restaurant()
    : currentTimeStep(0), TH(0),
      totalCNCount(0), totalCSCount(0),
      totalScooterCount(0),
      countODG(0), countODN(0), countOT(0),
      countOVG(0), countOVC(0), countOVN(0),
      totalOverwaitCount(0)     // FIX Bug 10: running counter initialized here
{}

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
    case ODG: PEND_ODG.enqueue(pOrd);                        countODG++; break;
    case ODN: PEND_ODN.enqueue(pOrd);                        countODN++; break;
    case OT:  PEND_OT.enqueue(pOrd);                         countOT++;  break;
    case OVG: PEND_OVG.enqueue(pOrd, pOrd->getPriority());   countOVG++; break;
    case OVC: PEND_OVC.enqueue(pOrd);                        countOVC++; break;
    case OVN: PEND_OVN.enqueue(pOrd);                        countOVN++; break;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Cancellation helpers
// ─────────────────────────────────────────────────────────────────────────────
static bool CancelFromQueue(LinkedQueue<Order*>& q, int id,
                             LinkedQueue<Order*>& cancelledList)
{
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
    LinkedQueue<CookingEntry*> tmp;
    CookingEntry* e;
    bool found = false;
    while (COOKING.dequeue(e)) {
        if (!found && e->order->getID() == id && e->order->getType() == OVC) {
            // Release chef back to correct pool
            if (e->chef->getType() == CN) availableCN.enqueue(e->chef);
            else                          availableCS.enqueue(e->chef);
            e->chef->releaseOrder();
            CANCELLED.enqueue(e->order);
            delete e;
            found = true;
        } else {
            tmp.enqueue(e);
        }
    }
    while (tmp.dequeue(e)) COOKING.enqueue(e);
    return found;
}

// FIX Bug 4: removed in-service search — spec says cancellation only works
// for pending, cooking, and ready states. In-service means scooter is moving.
bool Restaurant::RemoveOrderOVC(int id) {
    if (CancelFromQueue(PEND_OVC,  id, CANCELLED)) return true;
    if (CancelFromQueue(READY_OVC, id, CANCELLED)) return true;
    if (CancelFromCookingOVC(id))                  return true;
    return false;   // not found in any cancellable state — silently ignore
}

// ─────────────────────────────────────────────────────────────────────────────
// Chef retrieval
// ─────────────────────────────────────────────────────────────────────────────
Chef* Restaurant::GetFreeChef(CHEF_TYPE preferred, bool allowFallback) {
    Chef* c = nullptr;
    LinkedQueue<Chef*>& primary  = (preferred == CN) ? availableCN : availableCS;
    LinkedQueue<Chef*>& fallback = (preferred == CN) ? availableCS : availableCN;
    if (primary.dequeue(c))                   return c;
    if (allowFallback && fallback.dequeue(c)) return c;
    return nullptr;
}

// ─────────────────────────────────────────────────────────────────────────────
// Assign a chef to an order → move to COOKING
// FIX Bug 3 (cook time): ceiling division (size + speed - 1) / speed
// ─────────────────────────────────────────────────────────────────────────────
void Restaurant::AssignChefToOrder(Order* ord, Chef* chef) {
    ord->setTA(currentTimeStep);
    chef->assignOrder(ord->getID());
    CookingEntry* entry = new CookingEntry();
    entry->order = ord;
    entry->chef  = chef;
    // FIX: ceiling division ensures correct cook time
    entry->remainingTime = (ord->getSize() + chef->getSpeed() - 1) / chef->getSpeed();
    COOKING.enqueue(entry);
}

// ─────────────────────────────────────────────────────────────────────────────
// Stage 1 — assign pending orders to chefs in spec priority order:
//   1) ODG  → CS only
//   2) ODN  → CN preferred, CS fallback
//   3) OT   → CN only
//   4) OVG  → CS only  (priority queue, not FCFS)
//   5) OVC  → CN preferred, CS fallback
//   6) OVN  → CN only
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
// Advance cooking: decrement timers, move finished orders to ready queues
// ─────────────────────────────────────────────────────────────────────────────
void Restaurant::AdvanceCooking() {
    LinkedQueue<CookingEntry*> stillCooking;
    CookingEntry* entry;

    while (COOKING.dequeue(entry)) {
        entry->remainingTime--;

        if (entry->remainingTime <= 0) {
            Order* ord = entry->order;
            Chef*  ch  = entry->chef;
            ord->setTR(currentTimeStep);

            // Accumulate chef busy time for utilization stat
            ch->addBusyTime(ord->getCookTime());
            ch->releaseOrder();
            if (ch->getType() == CN) availableCN.enqueue(ch);
            else                     availableCS.enqueue(ch);
            delete entry;

            switch (ord->getType()) {
            case ODG: READY_ODG.enqueue(ord); break;
            case ODN: READY_ODN.enqueue(ord); break;
            case OT:  READY_OT.enqueue(ord);  break;
            case OVG: READY_OVG.enqueue(ord); break;
            case OVC: READY_OVC.enqueue(ord); break;
            case OVN: READY_OVN.enqueue(ord); break;
            }
        } else {
            stillCooking.enqueue(entry);
        }
    }
    while (stillCooking.dequeue(entry)) COOKING.enqueue(entry);
}

// ─────────────────────────────────────────────────────────────────────────────
// Overwait check (bonus): if ready OVG waited > TH timesteps, move to overwait
// FIX Bug 10: increments totalOverwaitCount as each order goes overwait
// ─────────────────────────────────────────────────────────────────────────────
void Restaurant::CheckOverwait() {
    LinkedQueue<Order*> stillReady;
    Order* ord;
    while (READY_OVG.dequeue(ord)) {
        if (currentTimeStep - ord->getTR() > TH) {
            int waitPri = currentTimeStep - ord->getTQ();
            READY_OVG_OVERWAIT.enqueue(ord, waitPri);
            totalOverwaitCount++;   // FIX Bug 10
        } else {
            stillReady.enqueue(ord);
        }
    }
    while (stillReady.dequeue(ord)) READY_OVG.enqueue(ord);
}

// ─────────────────────────────────────────────────────────────────────────────
// Table best-fit: prefer partially-occupied shareable tables, then smallest fit
// ─────────────────────────────────────────────────────────────────────────────
Table* Restaurant::FindBestFitTable(int seats, bool sharingOk) {
    LinkedQueue<Table*> tmp;
    Table* t;
    Table* bestShared = nullptr;
    Table* bestFresh  = nullptr;

    while (availableTables.dequeue(t)) {
        if (t->getFreeSeats() >= seats) {
            bool isPartial = t->getFreeSeats() < t->getCapacity();
            if (isPartial && sharingOk) {
                if (!bestShared || t->getFreeSeats() < bestShared->getFreeSeats())
                    bestShared = t;
            } else if (!isPartial) {
                if (!bestFresh || t->getFreeSeats() < bestFresh->getFreeSeats())
                    bestFresh = t;
            }
        }
        tmp.enqueue(t);
    }

    Table* chosen = bestShared ? bestShared : bestFresh;
    while (tmp.dequeue(t)) {
        if (t != chosen) availableTables.enqueue(t);
    }
    return chosen;
}

// ─────────────────────────────────────────────────────────────────────────────
// Scooter selection: pick scooter with least cumulative distance
// ─────────────────────────────────────────────────────────────────────────────
Scooter* Restaurant::FindShortestDistanceScooter() {
    LinkedQueue<Scooter*> tmp;
    Scooter* s; Scooter* best = nullptr;
    while (availableScooters.dequeue(s)) {
        if (!best || s->getTotalDistance() < best->getTotalDistance()) best = s;
        tmp.enqueue(s);
    }
    while (tmp.dequeue(s)) {
        if (s != best) availableScooters.enqueue(s);
    }
    return best;
}

// ─────────────────────────────────────────────────────────────────────────────
// Assign dine-in orders to tables (ODG and ODN)
// FIX Bug 5: InServiceEntry stores seatsUsed so releaseSeats() works correctly
// ─────────────────────────────────────────────────────────────────────────────
void Restaurant::AssignDineInOrders(LinkedQueue<Order*>& readyList) {
    LinkedQueue<Order*> waiting;
    Order* ord;
    while (readyList.dequeue(ord)) {
        // Try sharing table first
        Table* best = FindBestFitTable(ord->getSeats(), ord->getCanShare());
      

        if (best) {
            ord->setTS(currentTimeStep);
            best->occupy(ord->getSeats(), ord->getDuration(), currentTimeStep);
            // Keep partially-occupied tables available for future sharing
            if (best->getFreeSeats() > 0)
                availableTables.enqueue(best);
            InServiceEntry* se = new InServiceEntry();
            se->order     = ord;
            se->scooter   = nullptr;
            se->table     = best;
            se->seatsUsed = ord->getSeats();   // FIX Bug 5: store seats used
            INSERVICE_LIST.enqueue(se);
        } else {
            waiting.enqueue(ord);
        }
    }
    while (waiting.dequeue(ord)) readyList.enqueue(ord);
}

// ─────────────────────────────────────────────────────────────────────────────
// Stage 2 — assign delivery orders to scooters
// Order of assignment: overwait OVG → OVC → OVG → OVN
// FIX Bug 6: recordDelivery() and addDistance() moved to FinishInServiceOrders()
// ─────────────────────────────────────────────────────────────────────────────
void Restaurant::AssignDeliveryBatch() {
    auto assignOne = [&](Order* ord) -> bool {
        Scooter* s = FindShortestDistanceScooter();
        if (!s) return false;
        ord->setTS(currentTimeStep);
        // NOTE: distance and recordDelivery counted in FinishInServiceOrders()
        InServiceEntry* se = new InServiceEntry();
        se->order     = ord;
        se->scooter   = s;
        se->table     = nullptr;
        se->seatsUsed = 0;
        INSERVICE_LIST.enqueue(se);
        return true;

    };

    // 1) Overwait OVG — highest priority (bonus)
    {
        Order* ord; int pri;
        LinkedQueue<Order*> waiting;
        while (READY_OVG_OVERWAIT.peek(ord, pri)) {
            READY_OVG_OVERWAIT.dequeue(ord, pri);
            if (!assignOne(ord)) { waiting.enqueue(ord); break; }
        }
        while (waiting.dequeue(ord))
            READY_OVG_OVERWAIT.enqueue(ord, currentTimeStep - ord->getTQ());
    }
    // 2) OVC first
    {
        LinkedQueue<Order*> waiting; Order* ord;
        while (READY_OVC.dequeue(ord)) {
            if (!assignOne(ord)) { waiting.enqueue(ord); break; }
        }
        while (waiting.dequeue(ord)) READY_OVC.enqueue(ord);
    }
    // 3) OVG
    {
        LinkedQueue<Order*> waiting; Order* ord;
        while (READY_OVG.dequeue(ord)) {
            if (!assignOne(ord)) { waiting.enqueue(ord); break; }
        }
        while (waiting.dequeue(ord)) READY_OVG.enqueue(ord);
    }
    // 4) OVN
    {
        LinkedQueue<Order*> waiting; Order* ord;
        while (READY_OVN.dequeue(ord)) {
            if (!assignOne(ord)) { waiting.enqueue(ord); break; }
        }
        while (waiting.dequeue(ord)) READY_OVN.enqueue(ord);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Stage 2 — assign all ready orders to resources
// FIX Bug 3 (OT packing): check currentTimeStep > ord->getTS() not just else
// ─────────────────────────────────────────────────────────────────────────────
void Restaurant::AssignReadyOrders() {
    // --- Takeaway: wait exactly 1 timestep for packing ---
    {
        LinkedQueue<Order*> stillWaiting; Order* ord;
        while (READY_OT.dequeue(ord)) {
            if (ord->getTS() == 0) {
                // Packing starts now
                ord->setTS(currentTimeStep);
                stillWaiting.enqueue(ord);
            } else if (currentTimeStep > ord->getTS()) {
                // FIX Bug 3: at least 1 full step has passed → customer picks up
                ord->setTF(currentTimeStep);
                FINISHED.enqueue(ord);
            } else {
                // Same step packing started → keep waiting
                stillWaiting.enqueue(ord);
            }
        }
        while (stillWaiting.dequeue(ord)) READY_OT.enqueue(ord);
    }

    // --- Dine-in ---
    AssignDineInOrders(READY_ODG);
    AssignDineInOrders(READY_ODN);

    // --- Check overwait before delivery ---
    CheckOverwait();

    // --- Delivery ---
    AssignDeliveryBatch();
}

// ─────────────────────────────────────────────────────────────────────────────
// Finish in-service orders; free tables/scooters; handle maintenance
// FIX Bug 1 (loop order): this is called FIRST in RunSimulation()
// FIX Bug 5 (table sharing): use releaseSeats() instead of freeTable()
// FIX Bug 6 (scooter stats): distance + recordDelivery counted here on completion
// FIX Bug 7 (debug cout): removed
// ─────────────────────────────────────────────────────────────────────────────
void Restaurant::FinishInServiceOrders() {
    LinkedQueue<InServiceEntry*> stillInService;
    InServiceEntry* entry;

    while (INSERVICE_LIST.dequeue(entry)) {
        Order*   ord  = entry->order;
        bool     done = false;
        ORD_TYPE t    = ord->getType();

        if (t == OVG || t == OVC || t == OVN) {
            // TF = TS + ceil(distance / speed)
            int travelTime = (ord->getDistance() + entry->scooter->getSpeed() - 1)
                           / entry->scooter->getSpeed();
            if (currentTimeStep >= ord->getTS() + travelTime) {
                ord->setTF(currentTimeStep);
                FINISHED.enqueue(ord);


                // FIX Bug 6: count distance AFTER delivery completes
                // both outward and return trip count toward "busy" distance
                entry->scooter->addDistance(ord->getDistance()); // to customer
                entry->scooter->addBusyTime(travelTime);     // round trip
                entry->scooter->recordDelivery();                // FIX Bug 6

                returningScooters.enqueue(entry->scooter);
                delete entry;
                done = true;
            }
        } else {
            // OD: TF = TS + duration
            if (currentTimeStep >= ord->getTS() + ord->getDuration()) {
                ord->setTF(currentTimeStep);
                FINISHED.enqueue(ord);

                // FIX Bug 5: releaseSeats only frees this order's seats
                bool fullyFree = entry->table->releaseSeats(entry->seatsUsed);
                // If fully free, already removed from availableTables when assigned
                // If still partially occupied, it was kept in availableTables
                // Either way, re-add it only if fully free (it was removed on assignment)
                if (fullyFree)
                    availableTables.enqueue(entry->table);
                // If not fully free: table is already in availableTables (sharing)

                delete entry;
                done = true;
            }
        }
        if (!done) stillInService.enqueue(entry);
    }
    while (stillInService.dequeue(entry)) INSERVICE_LIST.enqueue(entry);

    // ── Process returning scooters ───────────────────────────────────────────
    Scooter* s;
    while (returningScooters.dequeue(s)) {
        if (s->needsMaintenance()) {
            s->startMaintenance(currentTimeStep);
            maintenanceScooters.enqueue(s);
        } else {
            availableScooters.enqueue(s);
        }
    }

    // ── Release scooters whose maintenance is done ───────────────────────────
    LinkedQueue<Scooter*> stillMaintaining;
    while (maintenanceScooters.dequeue(s)) {
        if (s->maintenanceDone(currentTimeStep)) {
            s->finishMaintenance();
            availableScooters.enqueue(s);
        } else {
            stillMaintaining.enqueue(s);
        }
    }
    while (stillMaintaining.dequeue(s)) maintenanceScooters.enqueue(s);
}

// ─────────────────────────────────────────────────────────────────────────────
// Simulation done when every queue except FINISHED/CANCELLED is empty
// ─────────────────────────────────────────────────────────────────────────────
bool Restaurant::IsSimulationDone() const {
    return ACTIONS_LIST.isEmpty()
        && PEND_ODG.isEmpty()  && PEND_ODN.isEmpty()
        && PEND_OT.isEmpty()   && PEND_OVG.isEmpty()
        && PEND_OVC.isEmpty()  && PEND_OVN.isEmpty()
        && COOKING.isEmpty()
        && READY_ODG.isEmpty() && READY_ODN.isEmpty()
        && READY_OT.isEmpty()  && READY_OVG.isEmpty()
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
        } else break;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Load input file
// ─────────────────────────────────────────────────────────────────────────────
void Restaurant::LoadFromFile(const string& filename) {
    ifstream fin(filename);
    if (!fin) { cout << "Error opening input file: " << filename << "\n"; return; }

    int cnCount, csCount;  fin >> cnCount >> csCount;
    int cnSpeed, csSpeed;  fin >> cnSpeed >> csSpeed;
    int sCount, sSpeed;    fin >> sCount  >> sSpeed;
    int mainOrds, mainDur; fin >> mainOrds >> mainDur;
    int totalTables;       fin >> totalTables;

    // Table pairs: (count, capacity) summing to totalTables
    int tableID = 0, tablesRead = 0;
    while (tablesRead < totalTables) {
        int tCount, tCap; fin >> tCount >> tCap;
        for (int i = 0; i < tCount; i++)
            availableTables.enqueue(new Table(++tableID, tCap));
        tablesRead += tCount;
    }

    fin >> TH;   // overwait threshold

    totalCNCount = cnCount;
    totalCSCount = csCount;
    int chefID = 1;
    for (int i = 0; i < cnCount; i++) availableCN.enqueue(new Chef(chefID++, CN, cnSpeed));
    for (int i = 0; i < csCount; i++) availableCS.enqueue(new Chef(chefID++, CS, csSpeed));

    totalScooterCount = sCount;
    for (int i = 1; i <= sCount; i++)
        availableScooters.enqueue(new Scooter(i, sSpeed, mainOrds, mainDur));

    int M; fin >> M;

    for (int i = 0; i < M; i++) {
        char actionType; fin >> actionType;

        if (actionType == 'Q') {
            string typStr; int TQ, ID, sz; double price;
            fin >> typStr >> TQ >> ID >> sz >> price;

            ORD_TYPE ot;
            if      (typStr == "ODG") ot = ODG;
            else if (typStr == "ODN") ot = ODN;
            else if (typStr == "OT")  ot = OT;
            else if (typStr == "OVG") ot = OVG;
            else if (typStr == "OVC") ot = OVC;
            else                      ot = OVN;

            Order* ord = new Order(ID, ot, TQ);
            ord->setSize(sz);
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
// Save output file
// FIX Bug 2 (sort loop): maxOrd is NOT re-enqueued back into FINISHED mid-sort
// FIX Bug 9 (AccumulateStats): removed dead declaration; stats gathered inline
// FIX Bug 10 (overwait count): uses totalOverwaitCount running counter
// ─────────────────────────────────────────────────────────────────────────────
void Restaurant::SaveToFile(const string& filename) {
    ofstream fout(filename);
    if (!fout) { cout << "Error opening output file: " << filename << "\n"; return; }

    // Drain FINISHED into 'remaining' for sorting
    LinkedQueue<Order*> remaining;
    Order* ord;
    while (FINISHED.dequeue(ord)) remaining.enqueue(ord);

    int finCount = remaining.getCount();
    double sumTi = 0, sumTc = 0, sumTw = 0, sumTserv = 0;

    // Selection sort: find max TF, write it, remove it — O(n^2) fine for project scale
    for (int written = 0; written < finCount; written++) {
        LinkedQueue<Order*> scan;
        Order* maxOrd = nullptr;

        while (remaining.dequeue(ord)) {
            if (!maxOrd || ord->getTF() > maxOrd->getTF()) {
                if (maxOrd) scan.enqueue(maxOrd);   // put previous max back
                maxOrd = ord;
            } else {
                scan.enqueue(ord);
            }
        }

        // Write maxOrd to file
        fout << maxOrd->getTF()          << "\t"
             << maxOrd->getID()          << "\t"
             << maxOrd->getTQ()          << "\t"
             << maxOrd->getTA()          << "\t"
             << maxOrd->getTR()          << "\t"
             << maxOrd->getTS()          << "\t"
             << maxOrd->getIdleTime()    << "\t"
             << maxOrd->getCookTime()    << "\t"
             << maxOrd->getWaitTime()    << "\t"
             << maxOrd->getServiceTime() << "\n";

        sumTi    += maxOrd->getIdleTime();
        sumTc    += maxOrd->getCookTime();
        sumTw    += maxOrd->getWaitTime();
        sumTserv += maxOrd->getServiceTime();

        // FIX Bug 2: do NOT put maxOrd back — it has been written and is done
        // Restore scan → remaining for next iteration
        while (scan.dequeue(ord)) remaining.enqueue(ord);
    }

    int cancelCount = CANCELLED.getCount();
    int total       = finCount + cancelCount;

    // Scooter utilization: sum all scooter busy times
    int totalScooterBusy = 0;
    {
        LinkedQueue<Scooter*> tmp;
        Scooter* s;
        while (availableScooters.dequeue(s))   { totalScooterBusy += s->getTotalBusyTime(); tmp.enqueue(s); }
        while (maintenanceScooters.dequeue(s)) { totalScooterBusy += s->getTotalBusyTime(); tmp.enqueue(s); }
        while (returningScooters.dequeue(s))   { totalScooterBusy += s->getTotalBusyTime(); tmp.enqueue(s); }
        // restore (not strictly needed but keeps queues consistent)
    }

    // Chef utilization: sum all chef busy times
    int totalChefBusy = 0;
    {
        LinkedQueue<Chef*> tmp; Chef* c;
        while (availableCN.dequeue(c)) { totalChefBusy += c->getTotalBusyTime(); tmp.enqueue(c); }
        while (tmp.dequeue(c)) availableCN.enqueue(c);
        while (availableCS.dequeue(c)) { totalChefBusy += c->getTotalBusyTime(); tmp.enqueue(c); }
        while (tmp.dequeue(c)) availableCS.enqueue(c);
    }

    fout << "\n========== STATISTICS ==========\n";

    // 1) Order counts
    fout << "1) Total orders: " << total << "\n"
         << "   ODG: " << countODG << "  ODN: " << countODN
         << "  OT: "  << countOT  << "  OVG: " << countOVG
         << "  OVC: " << countOVC << "  OVN: " << countOVN << "\n";

    // 2) Chefs
    fout << "2) Total chefs: " << (totalCNCount + totalCSCount)
         << "   CN: " << totalCNCount << "  CS: " << totalCSCount << "\n";

    // 3) Scooters
    fout << "3) Total scooters: " << totalScooterCount << "\n";

    // 4) Finished / cancelled %
    double finPct    = total ? 100.0 * finCount    / total : 0.0;
    double cancelPct = total ? 100.0 * cancelCount / total : 0.0;
    fout << "4) Finished: " << finCount << " (" << finPct << "%)"
         << "   Cancelled: " << cancelCount << " (" << cancelPct << "%)\n";

    // 5) Overwait % — FIX Bug 10: uses running counter, not empty queue
    double owPct = finCount ? 100.0 * totalOverwaitCount / finCount : 0.0;
    fout << "5) Overwait orders: " << totalOverwaitCount
         << " (" << owPct << "% of finished)\n";

    // 6) Averages
    if (finCount > 0) {
        fout << "6) Averages (finished orders):\n"
             << "   Avg Ti    = " << sumTi    / finCount << "\n"
             << "   Avg Tc    = " << sumTc    / finCount << "\n"
             << "   Avg Tw    = " << sumTw    / finCount << "\n"
             << "   Avg Tserv = " << sumTserv / finCount << "\n";
    }

    // 7) Scooter utilization
    double scooterUtil = (totalScooterCount > 0 && currentTimeStep > 0)
        ? 100.0 * totalScooterBusy / (totalScooterCount * currentTimeStep) : 0.0;
    fout << "7) Scooter utilization: " << scooterUtil << "%\n";

    // 8) Chef utilization
    int    totalChefs = totalCNCount + totalCSCount;
    double chefUtil   = (totalChefs > 0 && currentTimeStep > 0)
        ? 100.0 * totalChefBusy / (totalChefs * currentTimeStep) : 0.0;
    fout << "8) Chef utilization:    " << chefUtil << "%\n";

    fout.close();
}

// ─────────────────────────────────────────────────────────────────────────────
// Main simulation loop
// FIX Bug 1: FinishInServiceOrders() called FIRST to free tables/scooters
//            before AssignReadyOrders() tries to use them
// ─────────────────────────────────────────────────────────────────────────────
void Restaurant::RunSimulation(UI* ui) {
    while (true) {
        currentTimeStep++;

        // FIX Bug 1: free resources first so they're available for assignment this step
        FinishInServiceOrders();

        ExecuteEvents(currentTimeStep);
        AssignPendingToChefs();
        AdvanceCooking();
        AssignReadyOrders();

        if (ui->GetMode() == MODE_INTR)
            ui->PrintAll(this, currentTimeStep);

        if (IsSimulationDone()) break;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Accessors for UI
// ─────────────────────────────────────────────────────────────────────────────
int  Restaurant::GetTimeStep() const { return currentTimeStep; }

LinkedQueue<Action*>&         Restaurant::GetActions()           { return ACTIONS_LIST; }
LinkedQueue<Order*>&          Restaurant::GetPendingODG()        { return PEND_ODG; }
LinkedQueue<Order*>&          Restaurant::GetPendingODN()        { return PEND_ODN; }
LinkedQueue<Order*>&          Restaurant::GetPendingOT()         { return PEND_OT; }
priQueue<Order*>&             Restaurant::GetPendingOVG()        { return PEND_OVG; }
LinkedQueue<Order*>&          Restaurant::GetPendingOVC()        { return PEND_OVC; }
LinkedQueue<Order*>&          Restaurant::GetPendingOVN()        { return PEND_OVN; }
LinkedQueue<CookingEntry*>&   Restaurant::GetCooking()           { return COOKING; }
LinkedQueue<Order*>&          Restaurant::GetReadyODG()          { return READY_ODG; }
LinkedQueue<Order*>&          Restaurant::GetReadyODN()          { return READY_ODN; }
LinkedQueue<Order*>&          Restaurant::GetReadyOT()           { return READY_OT; }
priQueue<Order*>&             Restaurant::GetReadyOVGOverwait()  { return READY_OVG_OVERWAIT; }
LinkedQueue<Order*>&          Restaurant::GetReadyOVG()          { return READY_OVG; }
LinkedQueue<Order*>&          Restaurant::GetReadyOVC()          { return READY_OVC; }
LinkedQueue<Order*>&          Restaurant::GetReadyOVN()          { return READY_OVN; }
LinkedQueue<InServiceEntry*>& Restaurant::GetInService()         { return INSERVICE_LIST; }
LinkedQueue<Order*>&          Restaurant::GetFinished()          { return FINISHED; }
LinkedQueue<Order*>&          Restaurant::GetCancelled()         { return CANCELLED; }
LinkedQueue<Chef*>&           Restaurant::GetAvailableCN()       { return availableCN; }
LinkedQueue<Chef*>&           Restaurant::GetAvailableCS()       { return availableCS; }
LinkedQueue<Scooter*>&        Restaurant::GetAvailableScooters() { return availableScooters; }
LinkedQueue<Scooter*>&        Restaurant::GetMaintenanceScooters(){ return maintenanceScooters; }
LinkedQueue<Scooter*>&        Restaurant::GetReturningScooters() { return returningScooters; }
LinkedQueue<Table*>&          Restaurant::GetAvailableTables()   { return availableTables; }
