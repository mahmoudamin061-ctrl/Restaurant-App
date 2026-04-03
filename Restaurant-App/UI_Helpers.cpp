#include <iostream>
using namespace std;

#include "UI_Helpers.h"
#include "Restaurant.h"

// ===================== ACTIONS =====================
void PrintActions(Restaurant* R)
{
    cout << "--------------- Actions List ================" << endl;

    auto& actions = R->GetActions();

    int count = 0;
    int total = actions.getCount();

    cout << total << " actions remaining: ";

    // print ONLY first 10 actions
    actions.printFirstN(10);

    cout << endl;
}

// ===================== PENDING =====================
void PrintPending(Restaurant* R)
{
    cout << "------------- Pending Orders -----------------" << endl;

    auto& pending = R->GetPending();

    cout << pending.getCount() << " Orders: ";

    pending.print();

    cout << endl;
}

// ===================== AVAILABLE CHEFS =====================
void PrintAvailableChefs(Restaurant* R)
{
    cout << "------------- Available Chefs -----------------" << endl;

    auto& chefs = R->GetAvailableChefs();

    cout << chefs.getCount() << " Chefs: ";

    chefs.print();

    cout << endl;
}

// ===================== COOKING =====================
void PrintCooking(Restaurant* R)
{
    cout << "------------- Cooking Orders -----------------" << endl;

    auto& cooking = R->GetCooking();

    cout << cooking.getCount() << " Orders: ";

    // Expected format: [OrderID, ChefID]
    cooking.print();

    cout << endl;
}

// ===================== READY =====================
void PrintReady(Restaurant* R)
{
    cout << "------------- Ready Orders -----------------" << endl;

    auto& ready = R->GetReady();

    cout << ready.getCount() << " Orders: ";

    ready.print();

    cout << endl;
}

// ===================== SCOOTERS =====================
void PrintScooters(Restaurant* R)
{
    cout << "------------- Available Scooters -----------------" << endl;

    auto& scooters = R->GetAvailableScooters();

    cout << scooters.getCount() << " Scooters: ";

    scooters.print();

    cout << endl;
}

// ===================== TABLES =====================
void PrintTables(Restaurant* R)
{
    cout << "------------- Available Tables -----------------" << endl;

    auto& tables = R->GetAvailableTables();

    cout << tables.getCount() << " Tables: ";

    // Expected format: [TableID, Capacity, FreeSeats]
    tables.print();

    cout << endl;
}

// ===================== IN SERVICE =====================
void PrintInService(Restaurant* R)
{
    cout << "------------- In-Service Orders -----------------" << endl;

    auto& inService = R->GetInService();

    cout << inService.getCount() << " Orders: ";

    // Expected: [OrderID, ScooterID/TableID]
    inService.print();

    cout << endl;
}

// ===================== MAINTENANCE =====================
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

// ===================== FINISHED =====================
void PrintFinished(Restaurant* R)
{
    cout << "------------- Finished Orders -----------------" << endl;

    auto& finished = R->GetFinished();

    cout << finished.getCount() << " Orders: ";

    // MUST be sorted descending by finish time (handled by Restaurant)
    finished.print();

    cout << endl;
}