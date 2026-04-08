#pragma once
#include <iostream>

enum ORD_TYPE { TYPE_NRM, TYPE_VGAN, TYPE_VIP, TYPE_ODG, TYPE_ODN, TYPE_OT, TYPE_OVG, TYPE_OVC, TYPE_OVN };


class Order {
private:
    int ID;             
    ORD_TYPE type;      
    int ArrivalTime;    
    int size;           
    double price;       
    int distance;      

public:
 
    Order(int id, ORD_TYPE t, int AT) : ID(id), type(t), ArrivalTime(AT), size(0), price(0.0), distance(0) {}

    int getID() const { return ID; }
    ORD_TYPE getType() const { return type; }
    int getArrivalTime() const { return ArrivalTime; }
    int getSize() const { return size; }
    double getPrice() const { return price; }
    int getDistance() const { return distance; }

    friend std::ostream& operator<<(std::ostream& os, const Order* ord) {
        if (ord) {
            
            if (ord->type == TYPE_NRM || ord->type == TYPE_ODN || ord->type == TYPE_OVN) {
                os << "[" << ord->ID << "]";
            }
            
            else if (ord->type == TYPE_VGAN) {
                os << "(" << ord->ID << ")";
            }
            
            else if (ord->type == TYPE_VIP || ord->type == TYPE_OVG || ord->type == TYPE_OVC || ord->type == TYPE_ODG) {
                os << "{" << ord->ID << "}";
            }
          
            else {
                os << ord->ID;
            }
        }
        return os;
    }
};