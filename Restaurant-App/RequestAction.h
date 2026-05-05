#ifndef REQUEST_ACTION_H_
#define REQUEST_ACTION_H_

#include "Action.h"
#include "Order.h"
#include <iostream>

class RequestAction : public Action {
private:
    Order* pOrder;
public:
    RequestAction(int AT, Restaurant* pR, Order* order)
        : Action(AT, pR), pOrder(order) {}

    virtual void Execute() override;

    // FIX: prints [ODG, 7, 1] as required by spec
    virtual void print() const override {
        if (pOrder)
            std::cout << "[" << pOrder->getTYPEStr()
                      << ", " << ActionTime
                      << ", " << pOrder->getID() << "]";
    }
};

#endif
