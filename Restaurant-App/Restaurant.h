#pragma once

#include <string>
#include "include/LinkedQueue.h"
#include "include/priQueue.h"
#include "Order.h"
#include "Chef.h"
#include "Scooter.h"
#include "Table.h"

class Action;
class UI;

// ??? Cooking entry: links an order to the chef preparing it ??????????????????
struct CookingEntry {
    Order* order;
    Chef* chef;
    int    remainingTime;

    friend std::ostream& operator<<(std::ostream& os, const CookingEntry* e) {
        if (e) os << "[" << e->order->getID() << ", C" << e->chef->getID() << "]";
        return os;
    }
};

// ??? In-service entry: links an order to its scooter or table ????????????????
struct InServiceEntry {
    Order* order;
    Scooter* scooter;
    Table* table;

    friend std::ostream& operator<<(std::ostream& os, const InServiceEntry* e) {
        if (e) {
            os << "[" << e->order->getID() << ", ";
            if (e->scooter) os << "S" << e->scooter->getID();
            else if (e->table) os << "T" << e->table->getID();
            os << "]";
        }
        return os;
    }
};

class Restaurant {
private:
    int currentTimeStep;
    int TH;             // overwait threshold (loaded from file)

    // ?? Simulation-wide counters for statistics ??????????????????????????????
    int totalCNBusyTime;
    int totalCSBusyTime;
    int totalCNCount;
    int totalCSCount;
    int totalScooterBusyTime;
    int totalScooterCount;

    // ?? Type counts for output stats ?????????????????????????????????????????
    int countODG, countODN, countOT, countOVG, countOVC, countOVN;

    // ?? Action queue ?????????????????????????????????????????????????????????
    LinkedQueue<Action*>         ACTIONS_LIST;

    // ?? Pending queues (one per order type) ??????????????????????????????????
    LinkedQueue<Order*>          PEND_ODG;
    LinkedQueue<Order*>          PEND_ODN;
    LinkedQueue<Order*>          PEND_OT;
    priQueue<Order*>             PEND_OVG;   // priority = weighted score
    LinkedQueue<Order*>          PEND_OVC;
    LinkedQueue<Order*>          PEND_OVN;

    // ?? Cooking queue ????????????????????????????????????????????????????????
    LinkedQueue<CookingEntry*>   COOKING;

    // ?? Ready queues ?????????????????????????????????????????????????????????
    LinkedQueue<Order*>          READY_ODG;
    LinkedQueue<Order*>          READY_ODN;
    LinkedQueue<Order*>          READY_OT;
    priQueue<Order*>             READY_OVG_OVERWAIT; // bonus: overwait OVG, priority = currentTime - TQ
    LinkedQueue<Order*>          READY_OVG;
    LinkedQueue<Order*>          READY_OVC;
    LinkedQueue<Order*>          READY_OVN;

    // ?? In-service / finished / cancelled ????????????????????????????????????
    LinkedQueue<InServiceEntry*> INSERVICE_LIST;
    LinkedQueue<Order*>          FINISHED;
    LinkedQueue<Order*>          CANCELLED;

    // ?? Resources ????????????????????????????????????????????????????????????
    LinkedQueue<Chef*>           availableCN;        // normal chefs
    LinkedQueue<Chef*>           availableCS;        // special chefs
    LinkedQueue<Scooter*>        availableScooters;
    LinkedQueue<Scooter*>        maintenanceScooters;
    LinkedQueue<Scooter*>        returningScooters;
    LinkedQueue<Table*>          availableTables;

    // ?? Private helpers ???????????????????????????????????????????????????????
    bool     CancelFromCookingOVC(int targetID);
    Chef* GetFreeChef(CHEF_TYPE preferred, bool allowFallback);
    void     AssignChefToOrder(Order* ord, Chef* chef);
    void     AssignPendingToChefs();
    void     AdvanceCooking();
    void     CheckOverwait();
    void     AssignReadyOrders();
    void     AssignDineInOrders(LinkedQueue<Order*>& readyList);
    void     AssignDeliveryBatch();
    void     FinishInServiceOrders();
    bool     IsSimulationDone() const;
    Table* FindBestFitTable(int seats, bool sharingOk);
    Scooter* FindShortestDistanceScooter();
    void     AccumulateStats();

public:
    Restaurant();
    ~Restaurant();

    void AddAction(Action* pAct);
    void AddOrder(Order* pOrd);
    bool RemoveOrderOVC(int id);       // cancels OVC orders only
    void ExecuteEvents(int currentTime);
    void RunSimulation(UI* ui);
    void LoadFromFile(const std::string& filename);
    void SaveToFile(const std::string& filename);
    int  GetTimeStep() const;

    // ?? Accessors for UI ??????????????????????????????????????????????????????
    LinkedQueue<Action*>& GetActions();
    LinkedQueue<Order*>& GetPendingODN();
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
    priQueue<Order*>& GetReadyOVGOverwait();
    LinkedQueue<InServiceEntry*>& GetInService();
    LinkedQueue<Order*>& GetFinished();
    LinkedQueue<Order*>& GetCancelled();
    LinkedQueue<Chef*>& GetAvailableCN();
    LinkedQueue<Chef*>& GetAvailableCS();
    LinkedQueue<Scooter*>& GetAvailableScooters();
    LinkedQueue<Scooter*>& GetMaintenanceScooters();
    LinkedQueue<Scooter*>& GetReturningScooters();
    LinkedQueue<Table*>& GetAvailableTables();
};
