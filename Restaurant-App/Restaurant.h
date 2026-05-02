#pragma once

#include "include/LinkedQueue.h"
#include "include/priQueue.h"
#include "Order.h"
#include "Chef.h"
#include "Scooter.h"
#include "Table.h"

class Action;
class UI;

struct CookingEntry {
    Order* order;
    Chef* chef;

    friend std::ostream& operator<<(std::ostream& os, const CookingEntry* e) {
        if (e) os << "[" << e->order->getID() << ", C" << e->chef->getID() << "]";
        return os;
    }
};

class Restaurant {
private:
    int currentTimeStep;

    LinkedQueue<Action*>      ACTIONS_LIST;

    LinkedQueue<Order*>       PEND_ODN;
    LinkedQueue<Order*>       PEND_ODG;
    LinkedQueue<Order*>       PEND_OT;
    LinkedQueue<Order*>       PEND_OVN;
    LinkedQueue<Order*>       PEND_OVC;
    priQueue<Order*>          PEND_OVG;

    LinkedQueue<CookingEntry*> COOKING;

    LinkedQueue<Order*>       READY_ODN;
    LinkedQueue<Order*>       READY_ODG;
    LinkedQueue<Order*>       READY_OT;
    LinkedQueue<Order*>       READY_OVN;
    LinkedQueue<Order*>       READY_OVC;
    LinkedQueue<Order*>       READY_OVG;

    LinkedQueue<Order*>       INSERVICE;
    LinkedQueue<Order*>       FINISHED;
    LinkedQueue<Order*>       CANCELLED;

    LinkedQueue<Chef*>        availableChefs;
    LinkedQueue<Scooter*>     availableScooters;
    LinkedQueue<Scooter*>     maintenanceScooters;
    LinkedQueue<Scooter*>     returningScooters;
    LinkedQueue<Table*>       availableTables;

    bool CancelFromCooking(int targetID);

public:
    Restaurant();
    ~Restaurant();

    void AddAction(Action* pAct);
    void AddOrder(Order* pOrd);
    bool RemoveOrder(int id);
    void ExecuteEvents(int currentTime);
    void RunSimulation(UI* ui);
    int  GetTimeStep() const;

    LinkedQueue<Action*>& GetActions();
    LinkedQueue<Order*>& GetPending();
    LinkedQueue<Order*>& GetPendingODG();
    priQueue<Order*>& GetPendingOVG();
    LinkedQueue<Order*>& GetPendingOT();
    LinkedQueue<Order*>& GetPendingOVN();
    LinkedQueue<Order*>& GetPendingOVC();
    LinkedQueue<CookingEntry*>& GetCooking();
    LinkedQueue<Order*>& GetReadyODN();
    LinkedQueue<Order*>& GetReadyODG();
    LinkedQueue<Order*>& GetReadyOT();
    LinkedQueue<Order*>& GetReadyOVN();
    LinkedQueue<Order*>& GetReadyOVC();
    LinkedQueue<Order*>& GetReadyOVG();
    LinkedQueue<Order*>& GetInService();
    LinkedQueue<Order*>& GetFinished();
    LinkedQueue<Order*>& GetCancelled();
    LinkedQueue<Chef*>& GetAvailableChefs();
    LinkedQueue<Scooter*>& GetAvailableScooters();
    LinkedQueue<Scooter*>& GetMaintenanceScooters();
    LinkedQueue<Scooter*>& GetReturningScooters();
    LinkedQueue<Table*>& GetAvailableTables();
};