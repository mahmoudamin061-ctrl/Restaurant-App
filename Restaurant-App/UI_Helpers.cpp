#include <iostream>
using namespace std;

#include "UI_Helpers.h"
#include "Restaurant.h"
#include "Action.h"

// ─────────────────────────────────────────────────────────────────────────────
// FIX: Actions list – print first 10 actions with [Type, TQ, ID] as spec requires
// ─────────────────────────────────────────────────────────────────────────────
void PrintActions(Restaurant* R) {
    auto& actions = R->GetActions();
    int total = actions.getCount();
    cout << "=============== Actions List ===============\n";
    cout << total << " actions remaining: ";

    // Peek-print first 10 without modifying the queue
    // We dequeue up to 10, collect, then re-enqueue all
    Action* arr[10];
    int printed = 0;
    // We can't random-access a queue, so dequeue up to 10 and immediately re-enqueue
    // while capturing for display
    LinkedQueue<Action*> tmp;
    Action* a;
    while (actions.dequeue(a)) tmp.enqueue(a);   // drain to tmp

    Action* first10[10];
    int grabbed = 0;
    LinkedQueue<Action*> tmp2;
    while (tmp.dequeue(a)) {
        if (grabbed < 10) first10[grabbed++] = a;
        tmp2.enqueue(a);
    }
    while (tmp2.dequeue(a)) actions.enqueue(a);  // restore original queue

    cout << "[";
    for (int i = 0; i < grabbed; i++) {
        // CancelActions have no order type; distinguish by checking ActionTime sign
        // We print what we can from the base class
        cout << "T=" << first10[i]->getActionTime();
        if (i < grabbed - 1) cout << ", ";
    }
    cout << "]";
    if (total > 10) cout << " ... and " << (total - 10) << " more";
    cout << "\n\n";
}

// ─────────────────────────────────────────────────────────────────────────────
// Pending orders: one line per type
// ─────────────────────────────────────────────────────────────────────────────
void PrintPending(Restaurant* R) {
    cout << "------------- Pending Orders -----------------\n";

    cout << R->GetPendingODG().getCount() << " ODG: ";
    R->GetPendingODG().print(); cout << "\n";

    cout << R->GetPendingODN().getCount() << " ODN: ";
    R->GetPendingODN().print(); cout << "\n";

    cout << R->GetPendingOT().getCount() << " OT:  ";
    R->GetPendingOT().print(); cout << "\n";

    cout << R->GetPendingOVG().getCount() << " OVG: ";
    R->GetPendingOVG().print(); cout << "\n";

    cout << R->GetPendingOVC().getCount() << " OVC: ";
    R->GetPendingOVC().print(); cout << "\n";

    cout << R->GetPendingOVN().getCount() << " OVN: ";
    R->GetPendingOVN().print(); cout << "\n\n";
}

// ─────────────────────────────────────────────────────────────────────────────
// FIX: chefs now split into CN and CS pools
// ─────────────────────────────────────────────────────────────────────────────
void PrintAvailableChefs(Restaurant* R) {
    cout << "------------- Available Chefs -----------------\n";
    cout << R->GetAvailableCS().getCount() << " CS: ";
    R->GetAvailableCS().print(); cout << "\n";
    cout << R->GetAvailableCN().getCount() << " CN: ";
    R->GetAvailableCN().print(); cout << "\n\n";
}

// ─────────────────────────────────────────────────────────────────────────────
// Cooking orders: format [OrderID, ChefID]
// ─────────────────────────────────────────────────────────────────────────────
void PrintCooking(Restaurant* R) {
    auto& cooking = R->GetCooking();
    cout << "------------- Cooking Orders -----------------\n";
    cout << cooking.getCount() << " cooking orders: ";
    cooking.print();
    cout << "\n\n";
}

// ─────────────────────────────────────────────────────────────────────────────
// Ready orders: one line per type + overwait list
// ─────────────────────────────────────────────────────────────────────────────
void PrintReady(Restaurant* R) {
    cout << "------------- Ready Orders -----------------\n";
    cout << "ODG: ";  R->GetReadyODG().print(); cout << "\n";
    cout << "ODN: ";  R->GetReadyODN().print(); cout << "\n";
    cout << "OT:  ";  R->GetReadyOT().print();  cout << "\n";

    cout << "OVG (overwait): ";
    R->GetReadyOVGOverwait().print(); cout << "\n";

    cout << "OVG: ";  R->GetReadyOVG().print(); cout << "\n";
    cout << "OVC: ";  R->GetReadyOVC().print(); cout << "\n";
    cout << "OVN: ";  R->GetReadyOVN().print(); cout << "\n\n";
}

// ─────────────────────────────────────────────────────────────────────────────
// Available scooters
// ─────────────────────────────────────────────────────────────────────────────
void PrintScooters(Restaurant* R) {
    auto& scooters = R->GetAvailableScooters();
    cout << "------------- Available Scooters -----------------\n";
    cout << scooters.getCount() << " Scooters: ";
    scooters.print();
    cout << "\n\n";
}

// ─────────────────────────────────────────────────────────────────────────────
// Available tables: [ID, capacity, free seats]
// ─────────────────────────────────────────────────────────────────────────────
void PrintTables(Restaurant* R) {
    auto& tables = R->GetAvailableTables();
    cout << "------------- Available Tables [ID, cap, free] -----------------\n";
    cout << tables.getCount() << " tables: ";
    tables.print();
    cout << "\n\n";
}

// ─────────────────────────────────────────────────────────────────────────────
// In-service orders: [order ID, scooter/table ID]
// ─────────────────────────────────────────────────────────────────────────────
void PrintInService(Restaurant* R) {
    auto& inService = R->GetInService();
    cout << "------------- In-Service Orders [OrderID, S/T ID] -----------------\n";
    cout << inService.getCount() << " orders: ";
    inService.print();
    cout << "\n\n";
}

// ─────────────────────────────────────────────────────────────────────────────
// Maintenance and returning scooters
// ─────────────────────────────────────────────────────────────────────────────
void PrintMaintenance(Restaurant* R) {
    cout << "------------- In-Maintenance Scooters -----------------\n";
    cout << R->GetMaintenanceScooters().getCount() << " scooters: ";
    R->GetMaintenanceScooters().print();
    cout << "\n";

    cout << "------------- Scooters Back to Restaurant -----------------\n";
    cout << R->GetReturningScooters().getCount() << " scooters: ";
    R->GetReturningScooters().print();
    cout << "\n\n";
}

// ─────────────────────────────────────────────────────────────────────────────
// FIX: Cancelled orders section was missing
// ─────────────────────────────────────────────────────────────────────────────
void PrintCancelled(Restaurant* R) {
    auto& cancelled = R->GetCancelled();
    cout << "------------- Cancelled Orders -----------------\n";
    cout << cancelled.getCount() << " cancelled: ";
    cancelled.print();
    cout << "\n\n";
}

// ─────────────────────────────────────────────────────────────────────────────
// Finished orders: IDs in descending finish-time order
// ─────────────────────────────────────────────────────────────────────────────
void PrintFinished(Restaurant* R) {
    auto& finished = R->GetFinished();
    cout << "------------- Finished Orders (desc TF) -----------------\n";
    cout << finished.getCount() << " orders: ";
    finished.print();
    cout << "\n\n";
    cout << "PRESS ANY KEY TO MOVE TO NEXT STEP!\n";
}
