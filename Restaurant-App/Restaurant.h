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

// ── Cooking entry: pairs an order with the chef cooking it ──────────────────
struct CookingEntry {
    Order* order;
    Chef*  chef;
    int    remainingTime;   // timesteps left until done (ceiling division)

    friend std::ostream& operator<<(std::ostream& os, const CookingEntry* e) {
        if (e) os << "[" << e->order->getID() << ", C" << e->chef->getID() << "]";
        return os;
    }
};

// ── In-service entry: pairs an order with its scooter OR table ──────────────
struct InServiceEntry {
    Order*   order;
    Scooter* scooter;
    Table*   table;
    int      seatsUsed;    // FIX: store seats so releaseSeats() works correctly

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
    int TH;                     // overwait threshold

    // ── Stats counters (all set in constructor) ──────────────────────────────
    int totalCNCount, totalCSCount;
    int totalScooterCount;
    int countODG, countODN, countOT, countOVG, countOVC, countOVN;
    int totalOverwaitCount;     // FIX: running count of orders that ever went overwait

    // ── Action queue ─────────────────────────────────────────────────────────
    LinkedQueue<Action*>          ACTIONS_LIST;

    // ── Pending queues (one per order type) ──────────────────────────────────
    LinkedQueue<Order*>           PEND_ODG;
    LinkedQueue<Order*>           PEND_ODN;
    LinkedQueue<Order*>           PEND_OT;
    priQueue<Order*>              PEND_OVG;    // sorted by weighted priority
    LinkedQueue<Order*>           PEND_OVC;
    LinkedQueue<Order*>           PEND_OVN;

    // ── Cooking queue ─────────────────────────────────────────────────────────
    LinkedQueue<CookingEntry*>    COOKING;

    // ── Ready queues ─────────────────────────────────────────────────────────
    LinkedQueue<Order*>           READY_ODG;
    LinkedQueue<Order*>           READY_ODN;
    LinkedQueue<Order*>           READY_OT;
    priQueue<Order*>              READY_OVG_OVERWAIT;  // bonus: priority = currentTime-TQ
    LinkedQueue<Order*>           READY_OVG;
    LinkedQueue<Order*>           READY_OVC;
    LinkedQueue<Order*>           READY_OVN;

    // ── In-service / finished / cancelled ────────────────────────────────────
    LinkedQueue<InServiceEntry*>  INSERVICE_LIST;
    LinkedQueue<Order*>           FINISHED;
    LinkedQueue<Order*>           CANCELLED;

    // ── Resources ────────────────────────────────────────────────────────────
    LinkedQueue<Chef*>            availableCN;
    LinkedQueue<Chef*>            availableCS;
    LinkedQueue<Scooter*>         availableScooters;
    LinkedQueue<Scooter*>         maintenanceScooters;
    LinkedQueue<Scooter*>         returningScooters;
    LinkedQueue<Table*>           availableTables;

    // ── Private helpers ───────────────────────────────────────────────────────
    bool     CancelFromCookingOVC(int id);
    Chef*    GetFreeChef(CHEF_TYPE preferred, bool allowFallback);
    void     AssignChefToOrder(Order* ord, Chef* chef);
    void     AssignPendingToChefs();
    void     AdvanceCooking();
    void     CheckOverwait();
    void     AssignReadyOrders();
    void     AssignDineInOrders(LinkedQueue<Order*>& readyList);
    void     AssignDeliveryBatch();
    void     FinishInServiceOrders();
    bool     IsSimulationDone() const;
    Table*   FindBestFitTable(int seats, bool sharingOk);
    Scooter* FindShortestDistanceScooter();

public:
    Restaurant();
    ~Restaurant();

    void AddAction(Action* pAct);
    void AddOrder(Order* pOrd);
    bool RemoveOrderOVC(int id);
    void ExecuteEvents(int currentTime);
    void RunSimulation(UI* ui);
    void LoadFromFile(const std::string& filename);
    void SaveToFile(const std::string& filename);
    int  GetTimeStep() const;

    // ── Accessors for UI ──────────────────────────────────────────────────────
    LinkedQueue<Action*>&         GetActions();
    LinkedQueue<Order*>&          GetPendingODG();
    LinkedQueue<Order*>&          GetPendingODN();
    LinkedQueue<Order*>&          GetPendingOT();
    priQueue<Order*>&             GetPendingOVG();
    LinkedQueue<Order*>&          GetPendingOVC();
    LinkedQueue<Order*>&          GetPendingOVN();
    LinkedQueue<CookingEntry*>&   GetCooking();
    LinkedQueue<Order*>&          GetReadyODG();
    LinkedQueue<Order*>&          GetReadyODN();
    LinkedQueue<Order*>&          GetReadyOT();
    priQueue<Order*>&             GetReadyOVGOverwait();
    LinkedQueue<Order*>&          GetReadyOVG();
    LinkedQueue<Order*>&          GetReadyOVC();
    LinkedQueue<Order*>&          GetReadyOVN();
    LinkedQueue<InServiceEntry*>& GetInService();
    LinkedQueue<Order*>&          GetFinished();
    LinkedQueue<Order*>&          GetCancelled();
    LinkedQueue<Chef*>&           GetAvailableCN();
    LinkedQueue<Chef*>&           GetAvailableCS();
    LinkedQueue<Scooter*>&        GetAvailableScooters();
    LinkedQueue<Scooter*>&        GetMaintenanceScooters();
    LinkedQueue<Scooter*>&        GetReturningScooters();
    LinkedQueue<Table*>&          GetAvailableTables();
};
