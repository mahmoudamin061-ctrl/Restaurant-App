#pragma once

#include "include/LinkedQueue.h"
#include "include/priQueue.h"
#include "Order.h"
#include "Chef.h"
#include "Scooter.h"
#include "Table.h"

class Action;
class UI;

class Restaurant {
private:
    int currentTimeStep;

    LinkedQueue<Action*> ACTIONS_LIST;

    LinkedQueue<Order*> PEND_ODN;
    LinkedQueue<Order*> PEND_ODG;
    LinkedQueue<Order*> PEND_OT;
    LinkedQueue<Order*> PEND_OVN;
    LinkedQueue<Order*> PEND_OVC;
    priQueue<Order*>    PEND_OVG;

    LinkedQueue<Order*> COOKING;
    LinkedQueue<Order*> READY;
    LinkedQueue<Order*> INSERVICE;
    LinkedQueue<Order*> FINISHED;
    LinkedQueue<Order*> CANCELLED;

    LinkedQueue<Chef*> availableChefs;
    LinkedQueue<Scooter*> availableScooters;
    LinkedQueue<Scooter*> maintenanceScooters;
    LinkedQueue<Scooter*> returningScooters;
    LinkedQueue<Table*> availableTables;

public:
    Restaurant();

    void AddAction(Action* pAct);
    void AddOrder(Order* pOrd);
    bool RemoveOrder(int id);
    void ExecuteEvents(int currentTime);
    void RandomSimulation(UI* ui);

    void GenerateRandomOrder(int currentTime, int& lastID);

    void RunSimulation(UI* ui);

    int GetTimeStep() const;

    LinkedQueue<Action*>& GetActions();

    LinkedQueue<Order*>& GetPending();
        LinkedQueue<Order*>& GetPendingVegan();
        priQueue<Order*>& GetPendingVIP();
        LinkedQueue<Order*>& GetPendingOT();
        LinkedQueue<Order*>& GetPendingOVN();
        LinkedQueue<Order*>& GetPendingOVC();
    
    LinkedQueue<Order*>& GetCooking();
    LinkedQueue<Order*>& GetReady();
    LinkedQueue<Order*>& GetInService();
    LinkedQueue<Order*>& GetFinished();

    LinkedQueue<Chef*>& GetAvailableChefs();

    LinkedQueue<Scooter*>& GetAvailableScooters();
    LinkedQueue<Scooter*>& GetMaintenanceScooters();
    LinkedQueue<Scooter*>& GetReturningScooters();

    LinkedQueue<Table*>& GetAvailableTables();

    ~Restaurant();
};