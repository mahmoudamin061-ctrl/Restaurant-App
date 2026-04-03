#ifndef REQUEST_ACTION_H_
#define REQUEST_ACTION_H_

#include "Action.h"
#include "Order.h"

class RequestAction : public Action {
private:
    int ID;
    ORD_TYPE type;

public:
    RequestAction(int AT, Restaurant* pR, ORD_TYPE T, int id);

    virtual void Execute() override;
};

#endif