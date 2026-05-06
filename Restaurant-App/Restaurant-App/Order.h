#pragma once
#include <iostream>
#include <string>

enum ORD_TYPE { ODG, ODN, OT, OVG, OVC, OVN };

class Order {
private:
    int      ID;
    ORD_TYPE type;
    int      TQ, TA, TR, TS, TF;   
    int      size;
    double   price;
    int      seats;
    int      duration;
    bool     canShare;
    int      distance;

public:
    Order(int id, ORD_TYPE t, int tq)
        : ID(id), type(t),
          TQ(tq), TA(0), TR(0), TS(0), TF(0),
          size(0), price(0.0),
          seats(0), duration(0), canShare(false), distance(0)
    {}

    int      getID()       const { return ID; }
    ORD_TYPE getType()     const { return type; }
    int      getTQ()       const { return TQ; }
    int      getTA()       const { return TA; }
    int      getTR()       const { return TR; }
    int      getTS()       const { return TS; }
    int      getTF()       const { return TF; }
    int      getSize()     const { return size; }
    double   getPrice()    const { return price; }
    int      getSeats()    const { return seats; }
    int      getDuration() const { return duration; }
    bool     getCanShare() const { return canShare; }
    int      getDistance() const { return distance; }

    void setSize(int s)     { size = s; }
    void setPrice(double p) { price = p; }
    void setSeats(int s)    { seats = s; }
    void setDuration(int d) { duration = d; }
    void setCanShare(bool c){ canShare = c; }
    void setDistance(int d) { distance = d; }
    void setTA(int t)       { TA = t; }
    void setTR(int t)       { TR = t; }
    void setTS(int t)       { TS = t; }
    void setTF(int t)       { TF = t; }

    int getIdleTime()    const { return (TA - TQ) + (TS - TR); }
    int getCookTime()    const { return TR - TA; }
    int getWaitTime()    const { return getIdleTime() + getCookTime(); }
    int getServiceTime() const { return TF - TS; }

   
    int getPriority() const {
        return (int)(price * 0.5) + (size * 10) + (distance / 100);
    }

    std::string getTYPEStr() const {
        switch (type) {
        case ODG: return "ODG";
        case ODN: return "ODN";
        case OT:  return "OT";
        case OVG: return "OVG";
        case OVC: return "OVC";
        case OVN: return "OVN";
        default:  return "???";
        }
    }

    void print() const {
        std::cout << "[" << ID << " " << getTYPEStr() << " TQ=" << TQ << "]";
    }

    friend std::ostream& operator<<(std::ostream& os, const Order* ord) {
        if (ord) os << ord->ID;
        return os;
    }
};
