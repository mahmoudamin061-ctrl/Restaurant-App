#include "Restaurant.h"
#include "Action.h"
#include "UI.h"

// ===================== Constructor =====================
Restaurant::Restaurant()
{
    currentTimeStep = 0;
}

// ===================== Add Action =====================
void Restaurant::AddAction(Action* pAct)
{
    if (pAct)
        ACTIONS_LIST.enqueue(pAct);
}

// ===================== Add Order =====================
void Restaurant::AddOrder(Order* pOrd)
{
    if (!pOrd) return;

    switch (pOrd->getType())
    {
    case TYPE_ODG: PEND_ODG.enqueue(pOrd); break;
    case TYPE_ODN: PEND_ODN.enqueue(pOrd); break;
    case TYPE_OT:  PEND_OT.enqueue(pOrd); break;
    case TYPE_OVN: PEND_OVN.enqueue(pOrd); break;
    case TYPE_OVC: PEND_OVC.enqueue(pOrd); break;
    case TYPE_OVG: PEND_OVG.enqueue(pOrd, 10); break;
    default: break;
    }
}

// ===================== Execute Events =====================
void Restaurant::ExecuteEvents(int currentTime)
{
    Action* pAct = nullptr;

    while (ACTIONS_LIST.peek(pAct))
    {
        if (pAct->getActionTime() == currentTime)
        {
            ACTIONS_LIST.dequeue(pAct);
            pAct->Execute();
            delete pAct;
        }
        else break;
    }
}

// ===================== Remove Order =====================
bool Restaurant::RemoveOrder(int id)
{
    Order* temp = nullptr;
    LinkedQueue<Order*> helper;
    bool found = false;

    while (PEND_OVC.dequeue(temp))
    {
        if (temp->getID() == id && !found)
        {
            CANCELLED.enqueue(temp);
            found = true;
        }
        else
        {
            helper.enqueue(temp);
        }
    }

    while (helper.dequeue(temp))
        PEND_OVC.enqueue(temp);

    return found;
}

// ===================== SIMULATION =====================
void Restaurant::RunSimulation(UI* ui)
{
    ui->SelectMode();

    while (true)
    {
        ExecuteEvents(currentTimeStep);

        // ===== Phase 1.2 Dummy Movement =====
        Order* ord;

        // Pending ? Cooking
        if (PEND_ODN.dequeue(ord))
            COOKING.enqueue(ord);

        // Cooking ? Ready
        else if (COOKING.dequeue(ord))
            READY.enqueue(ord);

        // Ready ? InService
        else if (READY.dequeue(ord))
            INSERVICE.enqueue(ord);

        // InService ? Finished
        else if (INSERVICE.dequeue(ord))
            FINISHED.enqueue(ord);

        // ===== UI =====
        ui->PrintAll(this);

        currentTimeStep++;

        // Stop condition (Phase 1.2)
        if (ACTIONS_LIST.isEmpty() &&
            PEND_ODN.isEmpty() &&
            COOKING.isEmpty() &&
            READY.isEmpty() &&
            INSERVICE.isEmpty())
        {
            break;
        }
    }
}

// ===================== GETTERS =====================

int Restaurant::GetTimeStep() const
{
    return currentTimeStep;
}

LinkedQueue<Action*>& Restaurant::GetActions()
{
    return ACTIONS_LIST;
}

// ?? TEMP: return only one pending queue (Phase 1.2)
LinkedQueue<Order*>& Restaurant::GetPending()
{
    return PEND_ODN;
}

LinkedQueue<Order*>& Restaurant::GetCooking()
{
    return COOKING;
}

LinkedQueue<Order*>& Restaurant::GetReady()
{
    return READY;
}

LinkedQueue<Order*>& Restaurant::GetInService()
{
    return INSERVICE;
}

LinkedQueue<Order*>& Restaurant::GetFinished()
{
    return FINISHED;
}

// ===================== RESOURCES =====================
LinkedQueue<Chef*>& Restaurant::GetAvailableChefs()
{
    return availableChefs;
}

LinkedQueue<Scooter*>& Restaurant::GetAvailableScooters()
{
    return availableScooters;
}

LinkedQueue<Scooter*>& Restaurant::GetMaintenanceScooters()
{
    return maintenanceScooters;
}

LinkedQueue<Scooter*>& Restaurant::GetReturningScooters()
{
    return returningScooters;
}

LinkedQueue<Table*>& Restaurant::GetAvailableTables()
{
    return availableTables;
}

// ===================== Destructor =====================
Restaurant::~Restaurant() {}