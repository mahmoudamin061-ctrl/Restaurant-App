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
<<<<<<< HEAD

    switch (pOrd->getType()) {
    case ODG: cout << "[ODG-" << pOrd->getID() << "] "; break;
    case ODN: cout << "[ODN-" << pOrd->getID() << "] "; break;
    case OT:  cout << "[OT-" << pOrd->getID() << "] "; break;
    case OVG: cout << "[OVG-" << pOrd->getID() << "] "; break;
    case OVC: cout << "[OVC-" << pOrd->getID() << "] "; break;
    case OVN: cout << "[OVN-" << pOrd->getID() << "] "; break;
    default:  cout << "[?-" << pOrd->getID() << "] "; break;
    }
}
=======
    cout << pOrd->getID() << "(" << pOrd->getTYPEStr() << ") ";
}
>>>>>>> 6d0a9c1b1a6cef2dae5ee644f45da7f06554e4c4
