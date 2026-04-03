#include "Restaurant.h"
#include "Action.h"

Restaurant::Restaurant() {}

void Restaurant::AddAction(Action* pAct) {
    if (pAct)
        ACTIONS_LIST.enqueue(pAct);
}

void Restaurant::AddOrder(Order* pOrd) {
    if (!pOrd) return;

    switch (pOrd->getType()) {
    case TYPE_ODG:
        PEND_ODG.enqueue(pOrd);
        break;

    case TYPE_ODN:
        PEND_ODN.enqueue(pOrd);
        break;

    case TYPE_OT:
        PEND_OT.enqueue(pOrd);
        break;

    case TYPE_OVN:
        PEND_OVN.enqueue(pOrd);
        break;

    case TYPE_OVC:
        PEND_OVC.enqueue(pOrd);
        break;

    case TYPE_OVG:
        PEND_OVG.enqueue(pOrd, 10);
        break;

    case TYPE_NRM:
        PEND_ODN.enqueue(pOrd);
        break;

    case TYPE_VGAN:
        PEND_ODG.enqueue(pOrd);
        break;

    case TYPE_VIP:
        PEND_OVG.enqueue(pOrd, 10);
        break;

    default:
        break;
    }
}

void Restaurant::ExecuteEvents(int currentTime) {
    Action* pAct = nullptr;

    while (ACTIONS_LIST.peek(pAct)) {
        if (pAct->getActionTime() == currentTime) {
            ACTIONS_LIST.dequeue(pAct);
            pAct->Execute();
            delete pAct;
        }
        else {
            break;
        }
    }
}
bool Restaurant::RemoveOrder(int id) {
    Order* temp = nullptr;
    LinkedQueue<Order*> helper;
    bool found = false;

    while (PEND_OVC.dequeue(temp)) {
        if (temp->getID() == id && !found) {
            Cancelled_Orders.enqueue(temp);
            found = true;
        }
        else {
            helper.enqueue(temp);
        }
    }

    while (helper.dequeue(temp)) {
        PEND_OVC.enqueue(temp);
    }

    return found;
}

LinkedQueue<Order*>& Restaurant::getNormalPending() {
    return PEND_ODN;
}

LinkedQueue<Order*>& Restaurant::getVeganPending() {
    return PEND_ODG;
}

priQueue<Order*>& Restaurant::getVIPPending() {
    return PEND_OVG;
}

Restaurant::~Restaurant() {}
