#pragma once
#include <iostream>

enum CHEF_TYPE { CHEF_NRM, CHEF_VGAN, CHEF_VIP };


class Chef {
private:
    int ID;             
    CHEF_TYPE type = CHEF_NRM; 
    int speed;          

public:
    
    Chef(int id, CHEF_TYPE t, int s) : ID(id), type(t), speed(s) {}
    Chef() {
        ID = 0;
        speed = 1;
    }
    
    int getID() const { return ID; }

    
    friend std::ostream& operator<<(std::ostream& os, const Chef* c) {
        if (c) os << c->ID;
        return os;
    }
};