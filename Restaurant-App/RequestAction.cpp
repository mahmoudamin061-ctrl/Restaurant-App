#include "RequestAction.h"
#include "Restaurant.h"

RequestAction::RequestAction(int AT, Restaurant* pR, Order* order)
    : Action(AT, pR), pOrder(order) {
}

void RequestAction::Execute() {
    pRest->AddOrder(pOrder);
}
