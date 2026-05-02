#include "UI.h"
#include "Restaurant.h"
#include "UI_Helpers.h"
#include "Order.h"
#include <iostream>
#include <limits>
using namespace std;

UI::UI(UI_MODE m) : mode(m) {}

void UI::SelectMode() {
    cout << "Select Mode:\n  1. Interactive\n  2. Silent\nChoice: ";
    int choice; cin >> choice;
    mode = (choice == 1) ? MODE_INTR : MODE_SILENT;
    if (mode == MODE_SILENT)
        cout << "Simulation Starts in Silent mode ...\n";
}

int UI::GetMode() const { return mode; }

void UI::PrintHeader(int timestep) {
    cout << "\n===========================================\n";
    cout << "Current Timestep: " << timestep << "\n";
    cout << "===========================================\n";
}

void UI::Wait() {
    cout << "PRESS ANY KEY TO MOVE TO NEXT STEP!\n";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void UI::PrintAll(Restaurant* R, int currentTimeStep) {
    if (mode == MODE_SILENT) return;

    PrintHeader(currentTimeStep);
    PrintActions(R);
    PrintPending(R);
    PrintAvailableChefs(R);
    PrintCooking(R);
    PrintReady(R);
    PrintScooters(R);
    PrintTables(R);
    PrintInService(R);
    PrintMaintenance(R);
    PrintCancelled(R);    // was missing
    PrintFinished(R);

    Wait();
}

void UI::PrintOrderFormat(Order* pOrd) {
    if (!pOrd) return;
    cout << pOrd->getID() << "(" << pOrd->getTYPEStr() << ") ";
}
