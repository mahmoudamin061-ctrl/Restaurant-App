#ifndef REQUEST_ACTION_H_
#define REQUEST_ACTION_H_

#include "Action.h"
#include "Order.h"

class RequestAction : public Action {
private:
    Order* pOrder;

public:
    RequestAction(int AT, Restaurant* pR, Order* order);
    virtual void Execute() override;
};

#endif