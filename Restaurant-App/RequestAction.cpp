#include "RequestAction.h"
#include "Restaurant.h"

void RequestAction::Execute() {
    pRest->AddOrder(pOrder);
}
