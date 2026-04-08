#include "Restaurant.h"
#include "Action.h"
#include "UI.h"
#include <cstdlib>
#include <iostream> 

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


Restaurant::Restaurant()
{
    currentTimeStep = 0;
}


void Restaurant::AddAction(Action* pAct)
{
    if (pAct)
        ACTIONS_LIST.enqueue(pAct);
}



void Restaurant::GenerateRandomOrder(int currentTime, int& lastID) {

    int probability = rand() % 100;

    if (probability < 30) {
        lastID++;

        int typeRandom = rand() % 3;
        ORD_TYPE randomType;

        if (typeRandom == 0) randomType = TYPE_NRM;
        else if (typeRandom == 1) randomType = TYPE_VGAN;
        else randomType = TYPE_VIP;

        Order* newOrder = new Order(lastID, randomType, currentTime);
        AddOrder(newOrder);
    }
}

void Restaurant::AddOrder(Order* pOrd)
{
    if (!pOrd) return;

    switch (pOrd->getType())
    {
    case TYPE_NRM:  PEND_ODN.enqueue(pOrd); break;
    case TYPE_VGAN: PEND_ODG.enqueue(pOrd); break;
    case TYPE_VIP:  PEND_OVG.enqueue(pOrd, rand() % 100 + 1); break;
    case TYPE_OT:   PEND_OT.enqueue(pOrd); break;
    case TYPE_OVN:  PEND_OVN.enqueue(pOrd); break;
    case TYPE_OVC:  PEND_OVC.enqueue(pOrd); break;
    default: break;
    }
}

void Restaurant::ExecuteEvents(int currentTime)
{
    Action* pAct = nullptr;

    while (ACTIONS_LIST.peek(pAct))
    {
        if (pAct->getActionTime() == currentTime)
        {
            ACTIONS_LIST.dequeue(pAct);
            pAct->Execute();
            delete pAct;
        }
        else break;
    }
}

bool Restaurant::RemoveOrder(int id)
{
    Order* temp = nullptr;
    LinkedQueue<Order*> helper;
    bool found = false;

    while (PEND_ODN.dequeue(temp))
    {
        if (temp->getID() == id && !found)
        {
            CANCELLED.enqueue(temp);
            found = true;
        }
        else
        {
            helper.enqueue(temp);
        }
    }

    while (helper.dequeue(temp))
        PEND_ODN.enqueue(temp);

    return found;
}

void Restaurant::RunSimulation(UI* ui)
{
    ui->SelectMode();

    while (true)
    {
        ExecuteEvents(currentTimeStep);

        Order* ord;

        if (PEND_ODN.dequeue(ord))
            COOKING.enqueue(ord);

        else if (COOKING.dequeue(ord))
            READY.enqueue(ord);

        else if (READY.dequeue(ord))
            INSERVICE.enqueue(ord);

        else if (INSERVICE.dequeue(ord))
            FINISHED.enqueue(ord);

        ui->PrintAll(this, currentTimeStep);
        currentTimeStep++;

        if (ACTIONS_LIST.isEmpty() &&
            PEND_ODN.isEmpty() &&
            COOKING.isEmpty() &&
            READY.isEmpty() &&
            INSERVICE.isEmpty())
        {
            break;
        }
    }
}


int Restaurant::GetTimeStep() const
{
    return currentTimeStep;
}

LinkedQueue<Action*>& Restaurant::GetActions()
{
    return ACTIONS_LIST;
}
LinkedQueue<Order*>& Restaurant::GetPending()
{
    return PEND_ODN;
}

LinkedQueue<Order*>& Restaurant::GetPendingVegan()
{
    return PEND_ODG;
}

priQueue<Order*>& Restaurant::GetPendingVIP()
{
    return PEND_OVG;
}

LinkedQueue<Order*>& Restaurant::GetPendingOT()
{
    return PEND_OT;
}

LinkedQueue<Order*>& Restaurant::GetPendingOVN()
{
    return PEND_OVN;
}

LinkedQueue<Order*>& Restaurant::GetPendingOVC()
{
    return PEND_OVC;
}
LinkedQueue<Order*>& Restaurant::GetCooking()
{
    return COOKING;
}

LinkedQueue<Order*>& Restaurant::GetReady()
{
    return READY;
}

LinkedQueue<Order*>& Restaurant::GetInService()
{
    return INSERVICE;
}

LinkedQueue<Order*>& Restaurant::GetFinished()
{
    return FINISHED;
}

LinkedQueue<Chef*>& Restaurant::GetAvailableChefs()
{
    return availableChefs;
}

LinkedQueue<Scooter*>& Restaurant::GetAvailableScooters()
{
    return availableScooters;
}

LinkedQueue<Scooter*>& Restaurant::GetMaintenanceScooters()
{
    return maintenanceScooters;
}

LinkedQueue<Scooter*>& Restaurant::GetReturningScooters()
{
    return returningScooters;
}

LinkedQueue<Table*>& Restaurant::GetAvailableTables()
{
    return availableTables;
}

void Restaurant::RandomSimulation(UI* ui)
{
    ui->SelectMode(); 

   
    srand((unsigned int)time(0));

    
    for (int i = 1; i <= 100; i++) availableChefs.enqueue(new Chef(i, CHEF_NRM, 5));
    for (int i = 1; i <= 15; i++) availableScooters.enqueue(new Scooter(i, 10));
    for (int i = 1; i <= 15; i++) availableTables.enqueue(new Table(i, 5));

    for (int i = 1; i <= 500; i++) {
        int r = rand() % 6;
        ORD_TYPE randomType;
        if (r == 0) randomType = TYPE_NRM;
        else if (r == 1) randomType = TYPE_VGAN;
        else if (r == 2) randomType = TYPE_VIP;
        else if (r == 3) randomType = TYPE_OT;
        else if (r == 4) randomType = TYPE_OVN;
        else randomType = TYPE_OVC;

        Order* newOrder = new Order(i, randomType, 0); // ID: 1 to 500
        AddOrder(newOrder);
    }

    
    while (true)
    {
        currentTimeStep++;

        if (ui->GetMode() != MODE_SILENT) {
            cout << "\n--- Step " << currentTimeStep << " Actions Log ---\n";
        }

        for (int i = 0; i < 30; i++) {
            Order* ord = nullptr;
            Chef* chef = nullptr;

            int randList = rand() % 6;
            bool orderFound = false;

            if (randList == 0) orderFound = PEND_ODN.dequeue(ord);
            else if (randList == 1) orderFound = PEND_ODG.dequeue(ord);
            else if (randList == 2) {
                int dummyPri;
                orderFound = PEND_OVG.dequeue(ord, dummyPri);
            }
            else if (randList == 3) orderFound = PEND_OT.dequeue(ord);
            else if (randList == 4) orderFound = PEND_OVN.dequeue(ord);
            else if (randList == 5) orderFound = PEND_OVC.dequeue(ord);

            if (orderFound) {
                if (availableChefs.dequeue(chef)) {
                    COOKING.enqueue(ord);
                    if (ui->GetMode() != MODE_SILENT)
                        cout << "Action: Order " << ord->getID() << " moved to Cooking.\n";

                    delete chef;
                }
                else {
                    AddOrder(ord); 
                    break;
                }
            }
        }

       
        if ((rand() % 100) < 75) {
            for (int i = 0; i < 15; i++) {
                Order* ord = nullptr;
                if (COOKING.dequeue(ord)) {
                    READY.enqueue(ord);
                    if (ui->GetMode() != MODE_SILENT)
                        cout << "Action: Order " << ord->getID() << " is Ready.\n";
                }
                else {
                    break;
                }
            }
        }

        for (int i = 0; i < 10; i++) {
            Order* ord = nullptr;
            if (READY.dequeue(ord)) {
                ORD_TYPE t = ord->getType();
                if (t == TYPE_OT) {
                    FINISHED.enqueue(ord); 
                }
                else if (t == TYPE_OVN || t == TYPE_OVC || t == TYPE_VIP) {
                    Scooter* s = nullptr;
                    if (availableScooters.dequeue(s)) {
                        INSERVICE.enqueue(ord);
                        if (ui->GetMode() != MODE_SILENT)
                            cout << "Action: Order " << ord->getID() << " assigned to Scooter " << s->getID() << ".\n";

                       
                        delete s;
                    }
                    else {
                        READY.enqueue(ord);
                        break;
                    }
                }
                else {
                    Table* tb = nullptr;
                    if (availableTables.dequeue(tb)) {
                        INSERVICE.enqueue(ord);
                        if (ui->GetMode() != MODE_SILENT)
                            cout << "Action: Order " << ord->getID() << " assigned to Table " << tb->getID() << ".\n";

                        
                        delete tb;
                    }
                    else {
                        READY.enqueue(ord);
                        break;
                    }
                }
            }
            else {
                break;
            }
        }

      
        int randomCancelID = rand() % 500 + 1;
        if (CancelFromQueue(PEND_OVC, randomCancelID, CANCELLED)) {
            if (ui->GetMode() != MODE_SILENT) cout << "Action: Order " << randomCancelID << " Cancelled from PEND_OVC.\n";
        }

        randomCancelID = rand() % 500 + 1;
        if (CancelFromQueue(READY, randomCancelID, CANCELLED)) {
            if (ui->GetMode() != MODE_SILENT) cout << "Action: Order " << randomCancelID << " Cancelled from Ready.\n";
        }

       
        if ((rand() % 100) < 60) { 
            Order* ord = nullptr;
            if (INSERVICE.dequeue(ord)) {
                FINISHED.enqueue(ord);
                if (ui->GetMode() != MODE_SILENT) cout << "Action: Order " << ord->getID() << " Delivered!\n";

                availableChefs.enqueue(new Chef(rand() % 100 + 1, CHEF_NRM, 5));

                ORD_TYPE t = ord->getType();
                if (t == TYPE_OVN || t == TYPE_OVC || t == TYPE_VIP) {
                    returningScooters.enqueue(new Scooter(rand() % 15 + 1, 10)); 
                }
                else if (t != TYPE_OT) {
                    availableTables.enqueue(new Table(rand() % 15 + 1, 5)); 
                }
            }
        }

        if ((rand() % 100) < 80) {  
            Scooter* s = nullptr;
            if (maintenanceScooters.dequeue(s)) {
                availableScooters.enqueue(s);
            }
        }

        if ((rand() % 100) < 80) { 
            Scooter* s = nullptr;
            if (returningScooters.dequeue(s)) {
                if (rand() % 2 == 0) {
                    availableScooters.enqueue(s);
                }
                else {
                    maintenanceScooters.enqueue(s);
                }
            }
        }

        ui->PrintAll(this, currentTimeStep);

       
        bool allActiveEmpty = PEND_ODN.isEmpty() && PEND_ODG.isEmpty() && PEND_OVG.isEmpty() &&
            PEND_OT.isEmpty() && PEND_OVN.isEmpty() && PEND_OVC.isEmpty() &&
            COOKING.isEmpty() && READY.isEmpty() && INSERVICE.isEmpty();

        if (allActiveEmpty) {
            break;
        }
    }
}
Restaurant::~Restaurant() {}