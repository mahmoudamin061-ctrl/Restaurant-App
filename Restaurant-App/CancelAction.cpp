#include "CancelAction.h"
#include "Restaurant.h"

CancelAction::CancelAction(int AT, Restaurant* pR, int id)
    : Action(AT, pR), orderID(id)
{
}

void CancelAction::Execute() {
    pRest->RemoveOrder(orderID);
}
