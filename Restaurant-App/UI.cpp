#include "UI.h"
#include "Restaurant.h"
#include "UI_Helpers.h"
#include "Order.h" 

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
    cin.ignore();
    cin.get();
}

void UI::PrintAll(Restaurant* R, int currentTimeStep)
{
    if (mode == 2) return; 


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


void UI::PrintOrderFormat(Order* pOrd) {
    if (!pOrd) return;


    if (pOrd->getType() == TYPE_NRM) {
        cout << "[" << pOrd->getID() << "] ";
    }

    else if (pOrd->getType() == TYPE_VGAN) {
        cout << "(" << pOrd->getID() << ") ";
    }

    else if (pOrd->getType() == TYPE_VIP) {
        cout << "{" << pOrd->getID() << "} ";
    }
}