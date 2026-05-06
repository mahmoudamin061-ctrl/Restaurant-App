#pragma once
#include <iostream>

enum CHEF_TYPE { CN, CS };

class Chef {
private:
    int       ID;
    CHEF_TYPE type;
    int       speed;
    bool      busy;
    int       assignedOrderID;
    int       totalBusyTime;  

public:
    Chef(int id, CHEF_TYPE t, int s)
        : ID(id), type(t), speed(s),
          busy(false), assignedOrderID(-1), totalBusyTime(0)
    {}

    int       getID()             const { return ID; }
    CHEF_TYPE getType()           const { return type; }
    int       getSpeed()          const { return speed; }
    bool      isBusy()            const { return busy; }
	int       getAssignedOrder()  const { return assignedOrderID; } //lazem y3raf el order ID elly 3aleh
    int       getTotalBusyTime()  const { return totalBusyTime; }

    void addBusyTime(int t)   { totalBusyTime += t; }
    void assignOrder(int ordID) { busy = true;  assignedOrderID = ordID; }
	void releaseOrder() { busy = false; assignedOrderID = -1; } // lazem y3raf lama y5od order gedid aw y5las order elly 3aleh

    friend std::ostream& operator<<(std::ostream& os, const Chef* c) {
        if (c) os << c->ID;
        return os;
    }
};
