#include "CancelAction.h"
#include "Restaurant.h"

void CancelAction::Execute() {

    pRest->RemoveOrderOVC(orderID);
}
