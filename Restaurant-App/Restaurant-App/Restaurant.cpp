#include "Restaurant.h"
#include "Action.h"
#include "UI.h"
#include <iostream>
#include <fstream>
#include <string>
#include "RequestAction.h"
#include "CancelAction.h"

using namespace std;
Restaurant::Restaurant()
    : currentTimeStep(0), TH(0),
      totalCNCount(0), totalCSCount(0),
      totalScooterCount(0),
      countODG(0), countODN(0), countOT(0),
      countOVG(0), countOVC(0), countOVN(0),
      totalOverwaitCount(0)    
{}

Restaurant::~Restaurant() {}

//hola amingo
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

//
//void Restaurant::GenerateRandomOrder(int currentTime, int& lastID) {
//
//    int probability = rand() % 100;
//
//    if (probability < 30) {
//        lastID++;
//
//        int typeRandom = rand() % 3;
//        ORD_TYPE randomType;
//
//        if (typeRandom == 0) randomType = TYPE_NRM;
//        else if (typeRandom == 1) randomType = TYPE_VGAN;
//        else randomType = TYPE_VIP;
//
//        Order* newOrder = new Order(lastID, randomType, currentTime);
//        AddOrder(newOrder);
//    }
//}

bool Restaurant::CancelFromCookingOVC(int id) {
    LinkedQueue<CookingEntry*> tmp;
    CookingEntry* e;
    bool found = false;
    while (COOKING.dequeue(e)) {
        if (!found && e->order->getID() == id && e->order->getType() == OVC) {
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

bool Restaurant::RemoveOrderOVC(int id) {
    if (CancelFromQueue(PEND_OVC,  id, CANCELLED)) return true;
    if (CancelFromQueue(READY_OVC, id, CANCELLED)) return true;
    if (CancelFromCookingOVC(id))                  return true;
    return false;   
}

Chef* Restaurant::GetFreeChef(CHEF_TYPE preferred, bool allowFallback) {
    Chef* c = nullptr;
    LinkedQueue<Chef*>& primary  = (preferred == CN) ? availableCN : availableCS;
    LinkedQueue<Chef*>& fallback = (preferred == CN) ? availableCS : availableCN;
    if (primary.dequeue(c))                   return c;
    if (allowFallback && fallback.dequeue(c)) return c;
    return nullptr;
}


void Restaurant::AssignChefToOrder(Order* ord, Chef* chef) {
    ord->setTA(currentTimeStep);
    chef->assignOrder(ord->getID());
    CookingEntry* entry = new CookingEntry();
    entry->order = ord;
    entry->chef  = chef;
    entry->remainingTime = (ord->getSize() + chef->getSpeed() - 1) / chef->getSpeed();
    COOKING.enqueue(entry);
}


void Restaurant::AssignPendingToChefs() {
    Order* ord; int pri; Chef* chef;

    while (PEND_ODG.peek(ord)) {
        chef = GetFreeChef(CS, false);
        if (!chef) break;
        PEND_ODG.dequeue(ord);
        AssignChefToOrder(ord, chef);
    }
    while (PEND_ODN.peek(ord)) {
        chef = GetFreeChef(CN, true);
        if (!chef) break;
        PEND_ODN.dequeue(ord);
        AssignChefToOrder(ord, chef);
    }
    while (PEND_OT.peek(ord)) {
        chef = GetFreeChef(CN, false);
        if (!chef) break;
        PEND_OT.dequeue(ord);
        AssignChefToOrder(ord, chef);
    }
    while (PEND_OVG.peek(ord, pri)) {
        chef = GetFreeChef(CS, false);
        if (!chef) break;
        PEND_OVG.dequeue(ord, pri);
        AssignChefToOrder(ord, chef);
    }
    while (PEND_OVC.peek(ord)) {
        chef = GetFreeChef(CN, true);
        if (!chef) break;
        PEND_OVC.dequeue(ord);
        AssignChefToOrder(ord, chef);
    }
    while (PEND_OVN.peek(ord)) {
        chef = GetFreeChef(CN, false);
        if (!chef) break;
        PEND_OVN.dequeue(ord);
        AssignChefToOrder(ord, chef);
    }
}
//bos hena ya 3am el chef y5od el order w y7otoh fe el cooking w y7seb el remaining time w kol haga
void Restaurant::AdvanceCooking() {
    LinkedQueue<CookingEntry*> stillCooking;
    CookingEntry* entry;

    while (COOKING.dequeue(entry)) {
        entry->remainingTime--;

        if (entry->remainingTime <= 0) {
            Order* ord = entry->order;
            Chef*  ch  = entry->chef;
            ord->setTR(currentTimeStep);

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
//
//void Restaurant::RunSimulation(UI* ui)
//{
//    ui->SelectMode();
//
//    while (true)
//    {
//        ExecuteEvents(currentTimeStep);
//
//        Order* ord;
//
//        if (PEND_ODN.dequeue(ord))
//            COOKING.enqueue(ord);
//
//        else if (COOKING.dequeue(ord))
//            READY.enqueue(ord);
//
//        else if (READY.dequeue(ord))
//            INSERVICE.enqueue(ord);
//
//        else if (INSERVICE.dequeue(ord))
//            FINISHED.enqueue(ord);
//
//        ui->PrintAll(this, currentTimeStep);
//        currentTimeStep++;
//
//        if (ACTIONS_LIST.isEmpty() &&
//            PEND_ODN.isEmpty() &&
//            COOKING.isEmpty() &&
//            READY.isEmpty() &&
//            INSERVICE.isEmpty())
//        {
//            break;
//        }
//    }
//}

void Restaurant::CheckOverwait() {
    LinkedQueue<Order*> stillReady;
    Order* ord;
    while (READY_OVG.dequeue(ord)) {
        if (currentTimeStep - ord->getTR() > TH) {
            int waitPri = currentTimeStep - ord->getTQ();
            READY_OVG_OVERWAIT.enqueue(ord, waitPri);
            totalOverwaitCount++;   
        } else {
            stillReady.enqueue(ord);
        }
    }
    while (stillReady.dequeue(ord)) READY_OVG.enqueue(ord);
}
// tayb e7na hena 3ashan n3raf a7san table le dine in order 3ala 7asab el seats el fadiya w law sharing ok wla la2
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

void Restaurant::AssignDineInOrders(LinkedQueue<Order*>& readyList) {
    LinkedQueue<Order*> waiting;
    Order* ord;
    while (readyList.dequeue(ord)) {
        Table* best = FindBestFitTable(ord->getSeats(), ord->getCanShare());
      

        if (best) {
            ord->setTS(currentTimeStep);
            best->occupy(ord->getSeats(), ord->getDuration(), currentTimeStep);
            if (best->getFreeSeats() > 0)
                availableTables.enqueue(best);
            InServiceEntry* se = new InServiceEntry();
            se->order     = ord;
            se->scooter   = nullptr;
            se->table     = best;
            se->seatsUsed = ord->getSeats();  
            INSERVICE_LIST.enqueue(se);
        } else {
            waiting.enqueue(ord);
        }
    }
    while (waiting.dequeue(ord)) readyList.enqueue(ord);
}

void Restaurant::AssignDeliveryBatch() {
    auto assignOne = [&](Order* ord) -> bool {
        Scooter* s = FindShortestDistanceScooter();
        if (!s) return false;
        ord->setTS(currentTimeStep);
       
        InServiceEntry* se = new InServiceEntry();
        se->order     = ord;
        se->scooter   = s;
        se->table     = nullptr;
        se->seatsUsed = 0;
        INSERVICE_LIST.enqueue(se);
        return true;

    };

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
    {
        LinkedQueue<Order*> waiting; Order* ord;
        while (READY_OVC.dequeue(ord)) {
            if (!assignOne(ord)) { waiting.enqueue(ord); break; }
        }
        while (waiting.dequeue(ord)) READY_OVC.enqueue(ord);
    }
    {
        LinkedQueue<Order*> waiting; Order* ord;
        while (READY_OVG.dequeue(ord)) {
            if (!assignOne(ord)) { waiting.enqueue(ord); break; }
        }
        while (waiting.dequeue(ord)) READY_OVG.enqueue(ord);
    }
    {
        LinkedQueue<Order*> waiting; Order* ord;
        while (READY_OVN.dequeue(ord)) {
            if (!assignOne(ord)) { waiting.enqueue(ord); break; }
        }
        while (waiting.dequeue(ord)) READY_OVN.enqueue(ord);
    }
}

void Restaurant::AssignReadyOrders() {
    {
        LinkedQueue<Order*> stillWaiting; Order* ord;
        while (READY_OT.dequeue(ord)) {
            if (ord->getTS() == 0) {
                ord->setTS(currentTimeStep);
                stillWaiting.enqueue(ord);
            } else if (currentTimeStep > ord->getTS()) {
                ord->setTF(currentTimeStep);
                FINISHED.enqueue(ord);
            } else {
                stillWaiting.enqueue(ord);
            }
        }
        while (stillWaiting.dequeue(ord)) READY_OT.enqueue(ord);
    }

    AssignDineInOrders(READY_ODG);
    AssignDineInOrders(READY_ODN);

    CheckOverwait();

    AssignDeliveryBatch();
}


void Restaurant::FinishInServiceOrders() {
    LinkedQueue<InServiceEntry*> stillInService;
    InServiceEntry* entry;

    while (INSERVICE_LIST.dequeue(entry)) {
        Order*   ord  = entry->order;
        bool     done = false;
        ORD_TYPE t    = ord->getType();

        if (t == OVG || t == OVC || t == OVN) {
            int travelTime = (ord->getDistance() + entry->scooter->getSpeed() - 1)
                           / entry->scooter->getSpeed();
            if (currentTimeStep >= ord->getTS() + travelTime) {
                ord->setTF(currentTimeStep);
                FINISHED.enqueue(ord);


                entry->scooter->addDistance(ord->getDistance()); 
                entry->scooter->addBusyTime(travelTime);     
                entry->scooter->recordDelivery();               

                returningScooters.enqueue(entry->scooter);
                delete entry;
                done = true;
            }
        } else {
            if (currentTimeStep >= ord->getTS() + ord->getDuration()) {
                ord->setTF(currentTimeStep);
                FINISHED.enqueue(ord);

                bool fullyFree = entry->table->releaseSeats(entry->seatsUsed);
               
                if (fullyFree)
                    availableTables.enqueue(entry->table);

                delete entry;
                done = true;
            }
        }
        if (!done) stillInService.enqueue(entry);
    }
    while (stillInService.dequeue(entry)) INSERVICE_LIST.enqueue(entry);

    Scooter* s;
    while (returningScooters.dequeue(s)) {
        if (s->needsMaintenance()) {
            s->startMaintenance(currentTimeStep);
            maintenanceScooters.enqueue(s);
        } else {
            availableScooters.enqueue(s);
        }
    }

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

void Restaurant::LoadFromFile(const string& filename) {
    ifstream fin(filename);
    if (!fin) { cout << "Error opening input file: " << filename << "\n"; return; }

    int cnCount, csCount;  fin >> cnCount >> csCount;
    int cnSpeed, csSpeed;  fin >> cnSpeed >> csSpeed;
    int sCount, sSpeed;    fin >> sCount  >> sSpeed;
    int mainOrds, mainDur; fin >> mainOrds >> mainDur;
    int totalTables;       fin >> totalTables;

    int tableID = 0, tablesRead = 0;
    while (tablesRead < totalTables) {
        int tCount, tCap; fin >> tCount >> tCap;
        for (int i = 0; i < tCount; i++)
            availableTables.enqueue(new Table(++tableID, tCap));
        tablesRead += tCount;
    }

    fin >> TH;   

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
//
//void Restaurant::RandomSimulation(UI* ui)
//{
//    ui->SelectMode(); 
//
//   
//    srand((unsigned int)time(0));
//
//    
//    for (int i = 1; i <= 100; i++) availableChefs.enqueue(new Chef(i, CHEF_NRM, 5));
//    for (int i = 1; i <= 15; i++) availableScooters.enqueue(new Scooter(i, 10));
//    for (int i = 1; i <= 15; i++) availableTables.enqueue(new Table(i, 5));
//
//    for (int i = 1; i <= 500; i++) {
//        int r = rand() % 6;
//        ORD_TYPE randomType;
//        if (r == 0) randomType = TYPE_NRM;
//        else if (r == 1) randomType = TYPE_VGAN;
//        else if (r == 2) randomType = TYPE_VIP;
//        else if (r == 3) randomType = TYPE_OT;
//        else if (r == 4) randomType = TYPE_OVN;
//        else randomType = TYPE_OVC;
//
//        Order* newOrder = new Order(i, randomType, 0); // ID: 1 to 500
//        AddOrder(newOrder);
//    }
//
//    
//    while (true)
//    {
//        currentTimeStep++;
//
//        if (ui->GetMode() != MODE_SILENT) {
//            cout << "\n--- Step " << currentTimeStep << " Actions Log ---\n";
//        }
//
//        for (int i = 0; i < 30; i++) {
//            Order* ord = nullptr;
//            Chef* chef = nullptr;
//
//            int randList = rand() % 6;
//            bool orderFound = false;
//
//            if (randList == 0) orderFound = PEND_ODN.dequeue(ord);
//            else if (randList == 1) orderFound = PEND_ODG.dequeue(ord);
//            else if (randList == 2) {
//                int dummyPri;
//                orderFound = PEND_OVG.dequeue(ord, dummyPri);
//            }
//            else if (randList == 3) orderFound = PEND_OT.dequeue(ord);
//            else if (randList == 4) orderFound = PEND_OVN.dequeue(ord);
//            else if (randList == 5) orderFound = PEND_OVC.dequeue(ord);
//
//            if (orderFound) {
//                if (availableChefs.dequeue(chef)) {
//                    COOKING.enqueue(ord);
//                    if (ui->GetMode() != MODE_SILENT)
//                        cout << "Action: Order " << ord->getID() << " moved to Cooking.\n";
//
//                    delete chef;
//                }
//                else {
//                    AddOrder(ord); 
//                    break;
//                }
//            }
//        }
//
//       
//        if ((rand() % 100) < 75) {
//            for (int i = 0; i < 15; i++) {
//                Order* ord = nullptr;
//                if (COOKING.dequeue(ord)) {
//                    READY.enqueue(ord);
//                    if (ui->GetMode() != MODE_SILENT)
//                        cout << "Action: Order " << ord->getID() << " is Ready.\n";
//                }
//                else {
//                    break;
//                }
//            }
//        }
//
//        for (int i = 0; i < 10; i++) {
//            Order* ord = nullptr;
//            if (READY.dequeue(ord)) {
//                ORD_TYPE t = ord->getType();
//                if (t == TYPE_OT) {
//                    FINISHED.enqueue(ord); 
//                }
//                else if (t == TYPE_OVN || t == TYPE_OVC || t == TYPE_VIP) {
//                    Scooter* s = nullptr;
//                    if (availableScooters.dequeue(s)) {
//                        INSERVICE.enqueue(ord);
//                        if (ui->GetMode() != MODE_SILENT)
//                            cout << "Action: Order " << ord->getID() << " assigned to Scooter " << s->getID() << ".\n";
//
//                       
//                        delete s;
//                    }
//                    else {
//                        READY.enqueue(ord);
//                        break;
//                    }
//                }
//                else {
//                    Table* tb = nullptr;
//                    if (availableTables.dequeue(tb)) {
//                        INSERVICE.enqueue(ord);
//                        if (ui->GetMode() != MODE_SILENT)
//                            cout << "Action: Order " << ord->getID() << " assigned to Table " << tb->getID() << ".\n";
//
//                        
//                        delete tb;
//                    }
//                    else {
//                        READY.enqueue(ord);
//                        break;
//                    }
//                }
//            }
//            else {
//                break;
//            }
//        }
//
//      
//        int randomCancelID = rand() % 500 + 1;
//        if (CancelFromQueue(PEND_OVC, randomCancelID, CANCELLED)) {
//            if (ui->GetMode() != MODE_SILENT) cout << "Action: Order " << randomCancelID << " Cancelled from PEND_OVC.\n";
//        }
//
//        randomCancelID = rand() % 500 + 1;
//        if (CancelFromQueue(READY, randomCancelID, CANCELLED)) {
//            if (ui->GetMode() != MODE_SILENT) cout << "Action: Order " << randomCancelID << " Cancelled from Ready.\n";
//        }
//
//       
//        if ((rand() % 100) < 60) { 
//            Order* ord = nullptr;
//            if (INSERVICE.dequeue(ord)) {
//                FINISHED.enqueue(ord);
//                if (ui->GetMode() != MODE_SILENT) cout << "Action: Order " << ord->getID() << " Delivered!\n";
//
//                availableChefs.enqueue(new Chef(rand() % 100 + 1, CHEF_NRM, 5));
//
//                ORD_TYPE t = ord->getType();
//                if (t == TYPE_OVN || t == TYPE_OVC || t == TYPE_VIP) {
//                    returningScooters.enqueue(new Scooter(rand() % 15 + 1, 10)); 
//                }
//                else if (t != TYPE_OT) {
//                    availableTables.enqueue(new Table(rand() % 15 + 1, 5)); 
//                }
//            }
//        }
//
//        if ((rand() % 100) < 80) {  
//            Scooter* s = nullptr;
//            if (maintenanceScooters.dequeue(s)) {
//                availableScooters.enqueue(s);
//            }
//        }
//
//        if ((rand() % 100) < 80) { 
//            Scooter* s = nullptr;
//            if (returningScooters.dequeue(s)) {
//                if (rand() % 2 == 0) {
//                    availableScooters.enqueue(s);
//                }
//                else {
//                    maintenanceScooters.enqueue(s);
//                }
//            }
//        }
//
//        ui->PrintAll(this, currentTimeStep);
//
//       
//        bool allActiveEmpty = PEND_ODN.isEmpty() && PEND_ODG.isEmpty() && PEND_OVG.isEmpty() &&
//            PEND_OT.isEmpty() && PEND_OVN.isEmpty() && PEND_OVC.isEmpty() &&
//            COOKING.isEmpty() && READY.isEmpty() && INSERVICE.isEmpty();
//
//        if (allActiveEmpty) {
//            break;
//        }
//    }
//}

void Restaurant::SaveToFile(const string& filename) {
    ofstream fout(filename);
    if (!fout) { cout << "Error opening output file: " << filename << "\n"; return; }

    LinkedQueue<Order*> remaining;
    Order* ord;
    while (FINISHED.dequeue(ord)) remaining.enqueue(ord);

    int finCount = remaining.getCount();
    double sumTi = 0, sumTc = 0, sumTw = 0, sumTserv = 0;

    for (int written = 0; written < finCount; written++) {
        LinkedQueue<Order*> scan;
        Order* maxOrd = nullptr;

        while (remaining.dequeue(ord)) {
            if (!maxOrd || ord->getTF() > maxOrd->getTF()) {
                if (maxOrd) scan.enqueue(maxOrd); 
                maxOrd = ord;
            } else {
                scan.enqueue(ord);
            }
        }

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

        while (scan.dequeue(ord)) remaining.enqueue(ord);
    }

    int cancelCount = CANCELLED.getCount();
    int total       = finCount + cancelCount;

    int totalScooterBusy = 0;
    {
        LinkedQueue<Scooter*> tmp;
        Scooter* s;
        while (availableScooters.dequeue(s))   { totalScooterBusy += s->getTotalBusyTime(); tmp.enqueue(s); }
        while (maintenanceScooters.dequeue(s)) { totalScooterBusy += s->getTotalBusyTime(); tmp.enqueue(s); }
        while (returningScooters.dequeue(s))   { totalScooterBusy += s->getTotalBusyTime(); tmp.enqueue(s); }
    }

    int totalChefBusy = 0;
    {
        LinkedQueue<Chef*> tmp; Chef* c;
        while (availableCN.dequeue(c)) { totalChefBusy += c->getTotalBusyTime(); tmp.enqueue(c); }
        while (tmp.dequeue(c)) availableCN.enqueue(c);
        while (availableCS.dequeue(c)) { totalChefBusy += c->getTotalBusyTime(); tmp.enqueue(c); }
        while (tmp.dequeue(c)) availableCS.enqueue(c);
    }

    fout << "\n========== STATISTICS ==========\n";

    fout << "1) Total orders: " << total << "\n"
         << "   ODG: " << countODG << "  ODN: " << countODN
         << "  OT: "  << countOT  << "  OVG: " << countOVG
         << "  OVC: " << countOVC << "  OVN: " << countOVN << "\n";

    fout << "2) Total chefs: " << (totalCNCount + totalCSCount)
         << "   CN: " << totalCNCount << "  CS: " << totalCSCount << "\n";

    fout << "3) Total scooters: " << totalScooterCount << "\n";

    double finPct    = total ? 100.0 * finCount    / total : 0.0;
    double cancelPct = total ? 100.0 * cancelCount / total : 0.0;
    fout << "4) Finished: " << finCount << " (" << finPct << "%)"
         << "   Cancelled: " << cancelCount << " (" << cancelPct << "%)\n";

    double owPct = finCount ? 100.0 * totalOverwaitCount / finCount : 0.0;
    fout << "5) Overwait orders: " << totalOverwaitCount
         << " (" << owPct << "% of finished)\n";

    if (finCount > 0) {
        fout << "6) Averages (finished orders):\n"
             << "   Avg Ti    = " << sumTi    / finCount << "\n"
             << "   Avg Tc    = " << sumTc    / finCount << "\n"
             << "   Avg Tw    = " << sumTw    / finCount << "\n"
             << "   Avg Tserv = " << sumTserv / finCount << "\n";
    }

    double scooterUtil = (totalScooterCount > 0 && currentTimeStep > 0)
        ? 100.0 * totalScooterBusy / (totalScooterCount * currentTimeStep) : 0.0;
    fout << "7) Scooter utilization: " << scooterUtil << "%\n";

    int    totalChefs = totalCNCount + totalCSCount;
    double chefUtil   = (totalChefs > 0 && currentTimeStep > 0)
        ? 100.0 * totalChefBusy / (totalChefs * currentTimeStep) : 0.0;
    fout << "8) Chef utilization:    " << chefUtil << "%\n";

    fout.close();
}

void Restaurant::RunSimulation(UI* ui) {
    while (true) {
        currentTimeStep++;

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
