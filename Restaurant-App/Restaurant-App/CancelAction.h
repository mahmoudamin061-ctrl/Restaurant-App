#ifndef CANCEL_ACTION_H_
#define CANCEL_ACTION_H_

#include "Action.h"
#include <iostream>

class CancelAction : public Action {
private:
    int orderID;
public:
    CancelAction(int AT, Restaurant* pR, int id)
        : Action(AT, pR), orderID(id) {}

    virtual void Execute() override;

    virtual void print() const override {
        std::cout << "(X, " << ActionTime << ", " << orderID << ")";
    }
};

#endif
