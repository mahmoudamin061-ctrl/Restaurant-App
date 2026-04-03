#pragma once
#include <iostream>


enum ORD_TYPE { TYPE_NRM, TYPE_VGAN, TYPE_VIP };

class Order {
private:
    int ID;             
    ORD_TYPE type;      
    int ArrivalTime;    

public:
    
    Order(int id, ORD_TYPE t, int AT) : ID(id), type(t), ArrivalTime(AT) {}

    int getID() const { return ID; }

    
    friend std::ostream& operator<<(std::ostream& os, const Order* ord) {
        if (ord) os << ord->ID;
        return os;
    }
};