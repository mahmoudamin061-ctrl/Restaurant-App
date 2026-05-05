#include "CancelAction.h"
#include "Restaurant.h"

void CancelAction::Execute() {
    // Spec: cancellation is for OVC orders ONLY
    pRest->RemoveOrderOVC(orderID);
}
