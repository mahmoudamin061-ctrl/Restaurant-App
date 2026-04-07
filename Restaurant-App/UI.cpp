#include "UI.h"
#include "Restaurant.h"
#include "UI_Helpers.h"
#include "Order.h" 
#include <iostream>

using namespace std;

// ===================== Constructor =====================
UI::UI(UI_MODE UI_mode)
{
    mode = UI_mode;
}

// ===================== Select Mode =====================
void UI::SelectMode()
{
    cout << "Select Mode:\n1. Interactive\n2. Silent\n";

    int choice;
    cin >> choice;

    mode = (choice == 1) ? MODE_INTR : MODE_SILENT;
}

// ===================== Get Mode =====================
int UI::GetMode() const
{
    return mode;
}

// ===================== Print Header =====================
void UI::PrintHeader(int timestep)
{
    cout << "\nCurrent Timestep: " << timestep << endl;
}

// ===================== Wait =====================
void UI::Wait()
{
    cout << "Press any key to continue...\n";

    // Clear buffer to prevent skipping the first wait due to previous cin (Select Mode)
    if (cin.rdbuf()->in_avail() > 0) {
        cin.ignore(256, '\n');
    }
    cin.get();
}

// ===================== Print All =====================
void UI::PrintAll(Restaurant* R, int currentTimeStep)
{
    // Do not print anything in silent mode
    if (mode == 2) return;

    if (mode == MODE_SILENT) return;

    PrintHeader(currentTimeStep);

    // These functions should be implemented in UI_Helpers.cpp by Person 4
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

// ===================== Print Order Format =====================
void UI::PrintOrderFormat(Order* pOrd)
{
    if (!pOrd) return;

    switch (pOrd->getType()) {

        // ================= Phase 1 Basic Types =================
    case TYPE_NRM:
        cout << "[" << pOrd->getID() << "] ";
        break;
    case TYPE_VGAN:
        cout << "(" << pOrd->getID() << ") ";
        break;
    case TYPE_VIP:
        cout << "{" << pOrd->getID() << "} ";
        break;

        // ================= Phase 1.2 New Types =================
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