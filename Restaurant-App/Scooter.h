#pragma once
#include <iostream>

class Scooter {
private:
    int ID;
    int speed;
    int ordersDelivered;
    int mainOrdsThreshold;
    int mainDuration;
    int mainFinishTime;
    int totalDistance;

public:
    Scooter() : ID(0), speed(1), ordersDelivered(0),
        mainOrdsThreshold(0), mainDuration(0),
        mainFinishTime(0), totalDistance(0) {
    }

    Scooter(int id, int s, int mainOrds, int mainDur)
        : ID(id), speed(s), ordersDelivered(0),
        mainOrdsThreshold(mainOrds), mainDuration(mainDur),
        mainFinishTime(0), totalDistance(0) {
    }

    int  getID()             const { return ID; }
    int  getSpeed()          const { return speed; }
    int  getTotalDistance()  const { return totalDistance; }
    int  getMainFinishTime() const { return mainFinishTime; }

    void addDistance(int d) { totalDistance += d; }
    void recordDelivery() { ordersDelivered++; }
    bool needsMaintenance()  const { return ordersDelivered >= mainOrdsThreshold; }

    void startMaintenance(int currentTime) {
        ordersDelivered = 0;
        mainFinishTime = currentTime + mainDuration;
    }

    bool maintenanceDone(int currentTime) const {
        return mainFinishTime > 0 && currentTime >= mainFinishTime;
    }

    void finishMaintenance() { mainFinishTime = 0; }

    friend std::ostream& operator<<(std::ostream& os, const Scooter* s) {
        if (s) os << s->ID;
        return os;
    }
};
