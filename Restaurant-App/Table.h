#pragma once
#include <iostream>

enum TABLE_STATUS { EMPTY, OCCUPIED };

class Table {
private:
    int ID;
    int capacity;          
    TABLE_STATUS status;     

public:
  
    Table() {
        ID = 0;
        capacity = 1;
        status = EMPTY; 
    }
    Table(int id, int cap) : ID(id), capacity(cap), status(EMPTY) {}

    int getID() const { return ID; }

    friend std::ostream& operator<<(std::ostream& os, const Table* t) {
        if (t) os << t->ID;
        return os;
    }
};