#include "Restaurant.h"
#include "Action.h"
#include "UI.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include "RequestAction.h"
#include "CancelAction.h"

using namespace std;

bool CancelFromQueue(LinkedQueue<Order*>& q, int targetID, LinkedQueue<Order*>& cancelledList) {
    LinkedQueue<Order*> tempQ;
    Order* temp;
    bool found = false;

    while (q.dequeue(temp)) {
        if (temp->getID() == targetID && !found) {
            cancelledList.enqueue(temp);
            found = true;
        }
        else {
            tempQ.enqueue(temp);
        }
    }
    while (tempQ.dequeue(temp)) {
        q.enqueue(temp);
    }
    return found;
}

bool Restaurant::CancelFromCooking(int targetID) {
    LinkedQueue<CookingEntry*> temp;
    CookingEntry* entry;
    bool found = false;

    while (COOKING.dequeue(entry)) {
        if (entry->order->getID() == targetID && !found) {
            availableChefs.enqueue(entry->chef);
            CANCELLED.enqueue(entry->order);
            delete entry;
            found = true;
        }
        else {
            temp.enqueue(entry);
        }
    }
    while (temp.dequeue(entry))
        COOKING.enqueue(entry);

    return found;
}

bool Restaurant::RemoveOrder(int id) {
    if (CancelFromQueue(PEND_OVC, id, CANCELLED))
        return true;
    if (CancelFromQueue(READY_OVC, id, CANCELLED))
        return true;
    if (CancelFromCooking(id))
        return true;
    return false;
}

Restaurant::Restaurant() {
    currentTimeStep = 0;
}

void Restaurant::AddAction(Action* pAct) {
    if (pAct)
        ACTIONS_LIST.enqueue(pAct);
}

void Restaurant::AddOrder(Order* pOrd) {
    if (!pOrd) return;

    switch (pOrd->getType()) {
    case ODG: PEND_ODG.enqueue(pOrd);                         break;
    case ODN: PEND_ODN.enqueue(pOrd);                         break;
    case OT:  PEND_OT.enqueue(pOrd);                          break;
    case OVG: PEND_OVG.enqueue(pOrd, pOrd->getPriority());    break;
    case OVC: PEND_OVC.enqueue(pOrd);                         break;
    case OVN: PEND_OVN.enqueue(pOrd);                         break;
    }
}

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

void Restaurant::LoadFromFile(const string& filename) {
    ifstream fin(filename);
    if (!fin) {
        cout << "Error opening input file: " << filename << endl;
        return;
    }

    int cnCount, csCount;
    fin >> cnCount >> csCount;

    int cnSpeed, csSpeed;
    fin >> cnSpeed >> csSpeed;

    int sCount, sSpeed;
    fin >> sCount >> sSpeed;

    int mainOrds, mainDur;
    fin >> mainOrds >> mainDur;

    int totalTables;
    fin >> totalTables;

    int tablesRead = 0;
    while (tablesRead < totalTables) {
        int tCount, tCap;
        fin >> tCount >> tCap;
        for (int i = 0; i < tCount; i++) {
            availableTables.enqueue(new Table(++tablesRead, tCap));
        }
    }

    int TH;
    fin >> TH;

    int chefID = 1;
    for (int i = 0; i < cnCount; i++)
        availableChefs.enqueue(new Chef(chefID++, CN, cnSpeed));
    for (int i = 0; i < csCount; i++)
        availableChefs.enqueue(new Chef(chefID++, CS, csSpeed));

    for (int i = 1; i <= sCount; i++)
        availableScooters.enqueue(new Scooter(i, sSpeed, mainOrds));

    int M;
    fin >> M;

    for (int i = 0; i < M; i++) {
        char actionType;
        fin >> actionType;

        if (actionType == 'Q') {
            string typStr;
            int TQ, ID, size;
            double price;
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
                int seats, duration;
                char shareChar;
                fin >> seats >> duration >> shareChar;
                ord->setSeats(seats);
                ord->setDuration(duration);
                ord->setCanShare(shareChar == 'Y');
            }

            if (ot == OVG || ot == OVC || ot == OVN) {
                int distance;
                fin >> distance;
                ord->setDistance(distance);
            }

            AddAction(new RequestAction(TQ, this, ord));
        }
        else if (actionType == 'X') {
            int Tcancel, ID;
            fin >> Tcancel >> ID;
            AddAction(new CancelAction(Tcancel, this, ID));
        }
    }

    fin.close();
}

void Restaurant::SaveToFile(const string& filename) {
    ofstream fout(filename);
    if (!fout) {
        cout << "Error opening output file: " << filename << endl;
        return;
    }

    LinkedQueue<Order*> temp;
    Order* ord;

    while (FINISHED.dequeue(ord)) temp.enqueue(ord);

    LinkedQueue<Order*> remaining;
    while (temp.dequeue(ord)) remaining.enqueue(ord);

    while (!remaining.isEmpty()) {
        LinkedQueue<Order*> scan;
        Order* maxOrd = nullptr;

        while (remaining.dequeue(ord)) {
            if (!maxOrd || ord->getTF() > maxOrd->getTF())
                maxOrd = ord;
            scan.enqueue(ord);
        }

        fout << maxOrd->getTF() << " "
            << maxOrd->getID() << " "
            << maxOrd->getTQ() << " "
            << maxOrd->getTA() << " "
            << maxOrd->getTR() << " "
            << maxOrd->getTS() << " "
            << maxOrd->getIdleTime() << " "
            << maxOrd->getCookTime() << " "
            << maxOrd->getWaitTime() << " "
            << maxOrd->getServiceTime() << "\n";

        while (scan.dequeue(ord)) {
            if (ord != maxOrd)
                remaining.enqueue(ord);
            else
                FINISHED.enqueue(ord);
        }
    }

    int totalFinished = FINISHED.getCount();
    int totalCancelled = CANCELLED.getCount();
    int total = totalFinished + totalCancelled;

    fout << "\n--- Statistics ---\n";
    fout << "Total orders: " << total << "\n";
    fout << "Finished: " << totalFinished
        << " (" << (total ? totalFinished * 100 / total : 0) << "%)\n";
    fout << "Cancelled: " << totalCancelled
        << " (" << (total ? totalCancelled * 100 / total : 0) << "%)\n";

    fout.close();
}

void Restaurant::RunSimulation(UI* ui) {
    ui->SelectMode();

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

bool Restaurant::GetFreeChef(CHEF_TYPE type, Chef*& out) {
    LinkedQueue<Chef*> temp;
    Chef* c;
    bool found = false;

    while (availableChefs.dequeue(c)) {
        if (!found && c->getType() == type) {
            out = c;
            found = true;
        }
        else {
            temp.enqueue(c);
        }
    }
    while (temp.dequeue(c)) availableChefs.enqueue(c);
    return found;
}

void Restaurant::AssignChefToOrder(Order* ord, Chef* chef) {
    ord->setTA(currentTimeStep);
    chef->assignOrder(ord->getID());
    CookingEntry* entry = new CookingEntry();
    entry->order = ord;
    entry->chef = chef;
    entry->remainingTime = (ord->getSize() + chef->getSpeed() - 1) / chef->getSpeed();
    COOKING.enqueue(entry);
}

void Restaurant::AssignPendingToChefs() {
    Order* ord;
    Chef* chef;
    int    pri;

    while (PEND_ODG.peek(ord)) {
        if (!GetFreeChef(CS, chef)) break;
        PEND_ODG.dequeue(ord);
        AssignChefToOrder(ord, chef);
    }

    while (PEND_ODN.peek(ord)) {
        if (!GetFreeChef(CN, chef) && !GetFreeChef(CS, chef)) break;
        PEND_ODN.dequeue(ord);
        AssignChefToOrder(ord, chef);
    }

    while (PEND_OT.peek(ord)) {
        if (!GetFreeChef(CN, chef)) break;
        PEND_OT.dequeue(ord);
        AssignChefToOrder(ord, chef);
    }

    while (PEND_OVG.peek(ord, pri)) {
        if (!GetFreeChef(CS, chef)) break;
        PEND_OVG.dequeue(ord, pri);
        AssignChefToOrder(ord, chef);
    }

    while (PEND_OVC.peek(ord)) {
        if (!GetFreeChef(CN, chef) && !GetFreeChef(CS, chef)) break;
        PEND_OVC.dequeue(ord);
        AssignChefToOrder(ord, chef);
    }

    while (PEND_OVN.peek(ord)) {
        if (!GetFreeChef(CN, chef)) break;
        PEND_OVN.dequeue(ord);
        AssignChefToOrder(ord, chef);
    }
}

void Restaurant::AdvanceCooking() {
    LinkedQueue<CookingEntry*> stillCooking;
    CookingEntry* entry;

    while (COOKING.dequeue(entry)) {
        entry->remainingTime--;

        if (entry->remainingTime <= 0) {
            Order* ord = entry->order;
            Chef* chef = entry->chef;

            ord->setTR(currentTimeStep);
            chef->releaseOrder();
            availableChefs.enqueue(chef);
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
    while (stillCooking.dequeue(entry))
        COOKING.enqueue(entry);
}

void Restaurant::AssignReadyOrders() {
    Order* ord;
    LinkedQueue<Order*> stillWaiting;

    while (READY_OT.dequeue(ord)) {
        if (ord->getTS() == 0) {
            ord->setTS(currentTimeStep);
            stillWaiting.enqueue(ord);
        }
        else if (currentTimeStep > ord->getTS()) {
            ord->setTF(currentTimeStep);
            FINISHED.enqueue(ord);
        }
        else {
            stillWaiting.enqueue(ord);
        }
    }
    while (stillWaiting.dequeue(ord)) READY_OT.enqueue(ord);

    AssignDineInOrders(READY_ODG);
    AssignDineInOrders(READY_ODN);

    AssignDeliveryOrders(READY_OVC);
    AssignDeliveryOrders(READY_OVG);
    AssignDeliveryOrders(READY_OVN);
}

void Restaurant::AssignDineInOrders(LinkedQueue<Order*>& readyList) {
    LinkedQueue<Order*> waiting;
    Order* ord;

    while (readyList.dequeue(ord)) {
        Table* best = FindBestFitTable(ord->getSeats());
        if (best) {
            ord->setTS(currentTimeStep);
            best->occupy(ord->getSeats(), ord->getDuration(), currentTimeStep);
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

void Restaurant::AssignDeliveryOrders(LinkedQueue<Order*>& readyList) {
    LinkedQueue<Order*> waiting;
    Order* ord;

    while (readyList.dequeue(ord)) {
        Scooter* s = FindShortestDistanceScooter();
        if (s) {
            ord->setTS(currentTimeStep);
            s->addDistance(ord->getDistance() * 2);
            s->recordDelivery();
            InServiceEntry* se = new InServiceEntry();
            se->order = ord;
            se->scooter = s;
            se->table = nullptr;
            INSERVICE_LIST.enqueue(se);
        }
        else {
            waiting.enqueue(ord);
        }
    }
    while (waiting.dequeue(ord)) readyList.enqueue(ord);
}

void Restaurant::FinishInServiceOrders() {
    LinkedQueue<InServiceEntry*> stillInService;
    InServiceEntry* entry;

    while (INSERVICE_LIST.dequeue(entry)) {
        Order* ord = entry->order;
        bool done = false;
        ORD_TYPE t = ord->getType();

        if (t == OVG || t == OVC || t == OVN) {
            int travelTime = (ord->getDistance() / entry->scooter->getSpeed()) * 2;
            if (currentTimeStep >= ord->getTS() + travelTime) {
                ord->setTF(currentTimeStep);
                FINISHED.enqueue(ord);
                returningScooters.enqueue(entry->scooter);
                delete entry;
                done = true;
            }
        }
        else {
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

    Scooter* s;
    while (returningScooters.dequeue(s)) {
        if (s->needsMaintenance()) {
            s->resetMaintenance();
            maintenanceScooters.enqueue(s);
        }
        else {
            availableScooters.enqueue(s);
        }
    }
}

bool Restaurant::IsSimulationDone() const {
    return ACTIONS_LIST.isEmpty()
        && PEND_ODG.isEmpty() && PEND_ODN.isEmpty()
        && PEND_OT.isEmpty() && PEND_OVG.isEmpty()
        && PEND_OVC.isEmpty() && PEND_OVN.isEmpty()
        && COOKING.isEmpty()
        && READY_ODG.isEmpty() && READY_ODN.isEmpty()
        && READY_OT.isEmpty() && READY_OVG.isEmpty()
        && READY_OVC.isEmpty() && READY_OVN.isEmpty()
        && INSERVICE_LIST.isEmpty();
}

Table* Restaurant::FindBestFitTable(int seats) {
    LinkedQueue<Table*> temp;
    Table* t;
    Table* best = nullptr;

    while (availableTables.dequeue(t)) {
        if (t->hasRoom(seats)) {
            if (!best || t->getFreeSeats() < best->getFreeSeats())
                best = t;
        }
        temp.enqueue(t);
    }
    while (temp.dequeue(t)) availableTables.enqueue(t);
    return best;
}

Scooter* Restaurant::FindShortestDistanceScooter() {
    LinkedQueue<Scooter*> temp;
    Scooter* s;
    Scooter* best = nullptr;

    while (availableScooters.dequeue(s)) {
        if (!best || s->getTotalDistance() < best->getTotalDistance())
            best = s;
        temp.enqueue(s);
    }
    while (temp.dequeue(s)) availableScooters.enqueue(s);
    return best;
}

int Restaurant::GetTimeStep() const { return currentTimeStep; }

LinkedQueue<Action*>& Restaurant::GetActions() { return ACTIONS_LIST; }
LinkedQueue<Order*>& Restaurant::GetPending() { return PEND_ODN; }
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
LinkedQueue<InServiceEntry*>& Restaurant::GetInService() { return INSERVICE_LIST; }
LinkedQueue<Order*>& Restaurant::GetFinished() { return FINISHED; }
LinkedQueue<Order*>& Restaurant::GetCancelled() { return CANCELLED; }
LinkedQueue<Chef*>& Restaurant::GetAvailableChefs() { return availableChefs; }
LinkedQueue<Scooter*>& Restaurant::GetAvailableScooters() { return availableScooters; }
LinkedQueue<Scooter*>& Restaurant::GetMaintenanceScooters() { return maintenanceScooters; }
LinkedQueue<Scooter*>& Restaurant::GetReturningScooters() { return returningScooters; }
LinkedQueue<Table*>& Restaurant::GetAvailableTables() { return availableTables; }

Restaurant::~Restaurant() {}