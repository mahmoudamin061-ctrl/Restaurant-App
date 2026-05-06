#include <iostream>
using namespace std;

#include "UI_Helpers.h"
#include "Restaurant.h"
#include "Action.h"

void PrintActions(Restaurant* R) {
    auto& actions = R->GetActions();
    int total = actions.getCount();

    cout << "=============== Actions List ===============\n";

    cout << total << " actions remaining: ";

    LinkedQueue<Action*> tmp;
    Action* a;
    while (actions.dequeue(a)) tmp.enqueue(a);

    Action* first10[10];
    int grabbed = 0;
    LinkedQueue<Action*> tmp2;
    while (tmp.dequeue(a)) {
        if (grabbed < 10) first10[grabbed++] = a;
        tmp2.enqueue(a);
    }

    while (tmp2.dequeue(a)) actions.enqueue(a); 

    cout << "[";
    for (int i = 0; i < grabbed; i++) {
        first10[i]->print();                       
        if (i < grabbed - 1) cout << ", ";
    }
    cout << "]";
    if (total > 10) cout << " ... and " << (total - 10) << " more";
    cout << "\n\n";
}

void PrintPending(Restaurant* R) {
    cout << "------------- Pending Orders -----------------\n";
    cout << R->GetPendingODG().getCount() 
        << " ODG: "; R->GetPendingODG().print(); cout << "\n";
    cout << R->GetPendingODN().getCount()

        << " ODN: "; R->GetPendingODN().print(); cout << "\n";
    cout << R->GetPendingOT().getCount()            << " OT:  "; R->GetPendingOT().print();  cout << "\n";
    cout << R->GetPendingOVG().getCount()  << " OVG: "; R->GetPendingOVG().print(); cout << "\n";
    cout << R->GetPendingOVC().getCount() 
        << " OVC: "; R->GetPendingOVC().print(); cout << "\n";
    cout << R->GetPendingOVN().getCount() << " OVN: "; R->GetPendingOVN().print(); cout << "\n\n";
}

void PrintAvailableChefs(Restaurant* R) {
    cout << "------------- Available Chefs -----------------\n";
    cout << R->GetAvailableCS().getCount() << " CS: "; R->GetAvailableCS().print(); cout << "\n";
    cout << R->GetAvailableCN().getCount() << " CN: "; R->GetAvailableCN().print(); cout << "\n\n";
}

void PrintCooking(Restaurant* R) {
    auto& cooking = R->GetCooking();
    cout << "------------- Cooking Orders [OrderID, ChefID] -----------------\n";

    cout << cooking.getCount() << " cooking orders: ";
    cooking.print();

    cout << "\n\n";
}

void PrintReady(Restaurant* R) {
    cout << "------------- Ready Orders -----------------\n";
    cout << R->GetReadyODG().getCount()
        << " ODG: ";            R->GetReadyODG().print();         cout << "\n";
    cout << R->GetReadyODN().getCount()             << " ODN: ";            R->GetReadyODN().print();         cout << "\n";
    cout << R->GetReadyOT().getCount() << " OT:  ";            R->GetReadyOT().print();          cout << "\n";
    cout << R->GetReadyOVGOverwait().getCount() << " OVG (overwait): "; R->GetReadyOVGOverwait().print(); cout << "\n";
    cout << R->GetReadyOVG().getCount() 
        << " OVG: ";            R->GetReadyOVG().print();         cout << "\n";
    cout << R->GetReadyOVC().getCount() << " OVC: ";            R->GetReadyOVC().print();         cout << "\n";
    cout << R->GetReadyOVN().getCount() 
        
        
        << " OVN: ";            R->GetReadyOVN().print();         cout << "\n\n";
}

void PrintScooters(Restaurant* R) {

    cout << "------------- Available Scooters -----------------\n";
    cout << R->GetAvailableScooters().getCount() << " Scooters: ";
    R->GetAvailableScooters().print();  

    cout << "\n\n";
}

void PrintTables(Restaurant* R) {
    auto& tables = R->GetAvailableTables();
    cout << "------------- Available Tables [ID, cap, free] -----------------\n";
    cout << tables.getCount() << " tables: ";
    tables.print();
    cout << "\n\n";
}

void PrintInService(Restaurant* R) {
    auto& inService = R->GetInService();

    cout << "------------- In-Service Orders [OrderID, S/T ID] -----------------\n";
    cout << inService.getCount() << " orders: ";
    inService.print();

    cout << "\n\n";
}

void PrintMaintenance(Restaurant* R) {
    cout << "------------- In-Maintenance Scooters -----------------\n";
    cout << R->GetMaintenanceScooters().getCount() << " scooters: ";
      
    R->GetMaintenanceScooters().print(); cout << "\n";

    cout << "------------- Scooters Back to Restaurant -----------------\n";
    cout << R->GetReturningScooters().getCount() << " scooters: ";

    R->GetReturningScooters().print(); cout << "\n\n";
}

void PrintCancelled(Restaurant* R) {
    auto& cancelled = R->GetCancelled();
    cout << "------------- Cancelled Orders -----------------\n";

    cout << cancelled.getCount() << " cancelled: ";

    cancelled.print();
    cout << "\n\n";
}

void PrintFinished(Restaurant* R) {
    auto& finished = R->GetFinished();
    cout << "------------- Finished Orders (desc TF) -----------------\n";

     
       cout << finished.getCount() << " orders: ";
    finished.print();
    cout << "\n\n";
}
