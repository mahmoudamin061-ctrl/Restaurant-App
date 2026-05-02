#pragma once
#include <iostream>

class Scooter {
private:
    int ID;
    int speed;
    int ordersDelivered;
    int mainOrdsThreshold;
    int totalDistance;

public:
    Scooter() : ID(0), speed(1), ordersDelivered(0),
        mainOrdsThreshold(0), totalDistance(0) {
    }

    Scooter(int id, int s, int mainOrds)
        : ID(id), speed(s), ordersDelivered(0),
        mainOrdsThreshold(mainOrds), totalDistance(0) {
    }

    int getID()            const { return ID; }
    int getSpeed()         const { return speed; }
    int getTotalDistance() const { return totalDistance; }

    void addDistance(int d) { totalDistance += d; }
    void recordDelivery() { ordersDelivered++; }
    bool needsMaintenance() const { return ordersDelivered >= mainOrdsThreshold; }
    void resetMaintenance() { ordersDelivered = 0; }

    friend std::ostream& operator<<(std::ostream& os, const Scooter* s) {
        if (s) os << s->ID;
        return os;
    }
};