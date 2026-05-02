#include <iostream>
using namespace std;

#include "UI_Helpers.h"
#include "Restaurant.h"

void PrintActions(Restaurant* R)
{
    cout << "--------------- Actions List ================" << endl;

    auto& actions = R->GetActions();

    int count = 0;
    int total = actions.getCount();

    cout << total << " actions remaining: ";


    cout << endl;
}

void PrintPending(Restaurant* R)
{
    cout << "------------- Pending Orders -----------------" << endl;

    cout << R->GetPendingVegan().getCount() << " ODG: ";
    R->GetPendingVegan().print(); cout << endl;

    cout << R->GetPending().getCount() << " ODN: ";
    R->GetPending().print(); cout << endl;

    cout << R->GetPendingOT().getCount() << " OT:  ";
    R->GetPendingOT().print(); cout << endl;

    cout << R->GetPendingVIP().getCount() << " OVG: ";
    R->GetPendingVIP().print(); cout << endl;

    cout << R->GetPendingOVC().getCount() << " OVC: ";
    R->GetPendingOVC().print(); cout << endl;

    cout << R->GetPendingOVN().getCount() << " OVN: ";
    R->GetPendingOVN().print(); cout << endl;
}
void PrintAvailableChefs(Restaurant* R)
{
    cout << "------------- Available Chefs -----------------" << endl;

    auto& chefs = R->GetAvailableChefs();

    cout << chefs.getCount() << " Chefs: ";

    chefs.print();

    cout << endl;
}

void PrintCooking(Restaurant* R)
{
    cout << "------------- Cooking Orders -----------------" << endl;

    auto& cooking = R->GetCooking();

    cout << cooking.getCount() << " Orders: ";

    // Expected format: [OrderID, ChefID]
    cooking.print();

    cout << endl;
}

void PrintReady(Restaurant* R)
{
    cout << "------------- Ready Orders -----------------" << endl;

    cout << "ODN: ";  R->GetReadyODN().print(); cout << endl;
    cout << "ODG: ";  R->GetReadyODG().print(); cout << endl;
    cout << "OT:  ";  R->GetReadyOT().print();  cout << endl;
    cout << "OVN: ";  R->GetReadyOVN().print(); cout << endl;
    cout << "OVC: ";  R->GetReadyOVC().print(); cout << endl;
    cout << "OVG: ";  R->GetReadyOVG().print(); cout << endl;
}

void PrintScooters(Restaurant* R)
{
    cout << "------------- Available Scooters -----------------" << endl;

    auto& scooters = R->GetAvailableScooters();

    cout << scooters.getCount() << " Scooters: ";

    scooters.print();

    cout << endl;
}

void PrintTables(Restaurant* R)
{
    cout << "------------- Available Tables -----------------" << endl;

    auto& tables = R->GetAvailableTables();

    cout << tables.getCount() << " Tables: ";

    tables.print();

    cout << endl;
}

void PrintInService(Restaurant* R)
{
    cout << "------------- In-Service Orders -----------------" << endl;

    auto& inService = R->GetInService();

    cout << inService.getCount() << " Orders: ";

    inService.print();

    cout << endl;
}

void PrintMaintenance(Restaurant* R)
{
    cout << "------------- In-Maintenance Scooters -----------------" << endl;

    auto& maintenance = R->GetMaintenanceScooters();

    cout << maintenance.getCount() << " Scooters: ";

    maintenance.print();

    cout << endl;

    cout << "------------- Scooters Back to Restaurant -----------------" << endl;

    auto& returning = R->GetReturningScooters();

    cout << returning.getCount() << " Scooters: ";

    returning.print();

    cout << endl;
}

void PrintFinished(Restaurant* R)
{
    cout << "------------- Finished Orders -----------------" << endl;

    auto& finished = R->GetFinished();

    cout << finished.getCount() << " Orders: ";

    finished.print();

    cout << endl;
}