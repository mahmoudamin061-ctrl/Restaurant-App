#ifndef CANCEL_ACTION_H_
#define CANCEL_ACTION_H_

#include "Action.h"

class Restaurant;  

class CancelAction : public Action {
private:
    int orderID;

public:
    CancelAction(int AT, Restaurant* pR, int id);

    virtual void Execute() override;
};

#endif