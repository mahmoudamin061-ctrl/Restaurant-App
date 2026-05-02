#pragma once
#include <iostream>

class Table {
private:
    int ID;
    int capacity;
    int freeSeats;
    int busyUntil;

public:
    Table() : ID(0), capacity(1), freeSeats(1), busyUntil(0) {}
    Table(int id, int cap) : ID(id), capacity(cap), freeSeats(cap), busyUntil(0) {}

    int  getID()        const { return ID; }
    int  getCapacity()  const { return capacity; }
    int  getFreeSeats() const { return freeSeats; }
    int  getBusyUntil() const { return busyUntil; }

    bool hasRoom(int seats)   const { return freeSeats >= seats; }

    void occupy(int seats, int duration, int currentTime) {
        freeSeats -= seats;
        busyUntil = currentTime + duration;
    }

    void freeTable() { freeSeats = capacity; busyUntil = 0; }

    friend std::ostream& operator<<(std::ostream& os, const Table* t) {
        if (t) os << "[T" << t->ID << ", " << t->capacity << ", " << t->freeSeats << "]";
        return os;
    }
};
