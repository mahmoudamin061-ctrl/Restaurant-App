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

    bool hasRoom(int seats) const { return freeSeats >= seats; }

    // Called when an order is seated: reduces free seats, records when table is free
    void occupy(int seats, int duration, int currentTime) {
        freeSeats -= seats;
        // busyUntil tracks the LATEST finish time among all sharing orders
        int finishAt = currentTime + duration;
        if (finishAt > busyUntil) busyUntil = finishAt;
    }

    // FIX: releaseSeats only restores seats used by THIS order (for sharing)
    // Returns true if the table is now completely empty
    bool releaseSeats(int seats) {
        freeSeats += seats;
        if (freeSeats >= capacity) {
            freeSeats = capacity;
            busyUntil = 0;
            return true;   // table is now fully free
        }
        return false;      // still partially occupied by other sharing orders
    }

    // Full reset (used when a non-sharing order finishes and was sole occupant)
    void freeTable() { freeSeats = capacity; busyUntil = 0; }

    friend std::ostream& operator<<(std::ostream& os, const Table* t) {
        if (t) os << "[T" << t->ID << ", " << t->capacity << ", " << t->freeSeats << "]";
        return os;
    }
};
