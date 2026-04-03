#include "RequestAction.h"
#include "Restaurant.h"

RequestAction::RequestAction(int AT, Restaurant* pR, ORD_TYPE T, int id)
    : Action(AT, pR), type(T), ID(id)
{
}

void RequestAction::Execute() {
    Order* pOrd = new Order(ID, type, ActionTime);
    pRest->AddOrder(pOrd);
}