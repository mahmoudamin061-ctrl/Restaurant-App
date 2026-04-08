#include "UI.h"
#include "Restaurant.h"
#include "UI_Helpers.h"
#include "Order.h" 
#include <iostream>

using namespace std;

UI::UI(UI_MODE UI_mode)
{
    mode = UI_mode;
}

void UI::SelectMode()
{
    cout << "Select Mode:\n1. Interactive\n2. Silent\n";

    int choice;
    cin >> choice;

    mode = (choice == 1) ? MODE_INTR : MODE_SILENT;
}

int UI::GetMode() const
{
    return mode;
}

void UI::PrintHeader(int timestep)
{
    cout << "\nCurrent Timestep: " << timestep << endl;
}

void UI::Wait()
{
    cout << "Press any key to continue...\n";

    if (cin.rdbuf()->in_avail() > 0) {
        cin.ignore(256, '\n');
    }
    cin.get();
}

void UI::PrintAll(Restaurant* R, int currentTimeStep)
{
    if (mode == 2) return;

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
    PrintFinished(R);

    Wait();
}

void UI::PrintOrderFormat(Order* pOrd)
{
    if (!pOrd) return;

    switch (pOrd->getType()) {

    case TYPE_NRM:
        cout << "[" << pOrd->getID() << "] ";
        break;
    case TYPE_VGAN:
        cout << "(" << pOrd->getID() << ") ";
        break;
    case TYPE_VIP:
        cout << "{" << pOrd->getID() << "} ";
        break;

    case TYPE_OT:
        cout << "<" << pOrd->getID() << "> ";
        break;
    case TYPE_OVN:
        cout << "[[" << pOrd->getID() << "]] ";
        break;
    case TYPE_OVC:
        cout << "{{" << pOrd->getID() << "}} ";
        break;

    default:
        cout << pOrd->getID() << " ";
        break;
    }
}