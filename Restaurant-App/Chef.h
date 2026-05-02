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

public:
    Chef(int id, CHEF_TYPE t, int s)
        : ID(id), type(t), speed(s), busy(false), assignedOrderID(-1) {
    }

    int       getID()            const { return ID; }
    CHEF_TYPE getType()          const { return type; }
    int       getSpeed()         const { return speed; }
    bool      isBusy()           const { return busy; }
    int       getAssignedOrder() const { return assignedOrderID; }

    void assignOrder(int ordID) { busy = true;  assignedOrderID = ordID; }
    void releaseOrder() { busy = false; assignedOrderID = -1; }

    friend std::ostream& operator<<(std::ostream& os, const Chef* c) {
        if (c) os << c->ID;
        return os;
    }
};
