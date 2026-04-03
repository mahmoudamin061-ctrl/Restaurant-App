#ifndef RESTAURANT_H_
#define RESTAURANT_H_

#include "include/LinkedQueue.h"
#include "include/priQueue.h"
#include "include/LinkedList.h"
#include "Order.h"
#include "Chef.h"

class Action;

class Restaurant {
private:
    LinkedQueue<Action*> ACTIONS_LIST;

    LinkedQueue<Order*> PEND_ODN;
    LinkedQueue<Order*> PEND_ODG;
    LinkedQueue<Order*> PEND_OT;
    LinkedQueue<Order*> PEND_OVN;
    LinkedQueue<Order*> PEND_OVC;
    priQueue<Order*>    PEND_OVG;

    LinkedQueue<Chef*> Free_CN;
    LinkedQueue<Chef*> Free_CS;

    LinkedQueue<Order*> RDY_OD;
    LinkedQueue<Order*> RDY_OT;
    LinkedQueue<Order*> RDY_OV;

    LinkedQueue<Order*> Cancelled_Orders;
    LinkedQueue<Order*> Finished_Orders;

public:
    Restaurant();

    void AddAction(Action* pAct);
    void AddOrder(Order* pOrd);
    bool RemoveOrder(int id);
    void ExecuteEvents(int currentTime);

    LinkedQueue<Order*>& getNormalPending();
    LinkedQueue<Order*>& getVeganPending();
    priQueue<Order*>& getVIPPending();

    ~Restaurant();
};

#endif