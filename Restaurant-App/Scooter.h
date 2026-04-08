#pragma once
#include <iostream>

enum SCOOTER_STATUS { IDLE, DELIVERING };

class Scooter {
private:
    int ID;                 
    int speed;              
    SCOOTER_STATUS status;  
public:
    Scooter() {
        ID = 0;
        speed = 1;
        status = IDLE; 
    }
    Scooter(int id, int s) : ID(id), speed(s), status(IDLE) {}
   
    int getID() const { return ID; }

    friend std::ostream& operator<<(std::ostream& os, const Scooter* s) {
        if (s) os << s->ID;
        return os;
    }
};