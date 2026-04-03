#pragma once
#include <iostream>

// أنواع الأوردرات المتاحة في المطعم
enum ORD_TYPE { TYPE_NRM, TYPE_VGAN, TYPE_VIP, TYPE_ODG, TYPE_ODN,TYPE_OT, TYPE_OVG,TYPE_OVC,TYPE_OVN};
//TYPE_ODG-->Dine-in Grilled
//TYPE_ODN-->Dine-in Normal
//TYPE_OT--> Takeaway
//TYPE_OVG--> Delivery Grilled
//TYPE_OVC-->Delivery Cold
//TYPE_OVN --> Delivery Normal

class Order {
private:
    int ID;             // رقم الأوردر
    ORD_TYPE type;      // نوع الأوردر (عادي، نباتي، VIP)
    int ArrivalTime;    // وقت وصول الأوردر
    int size;           // عدد الأطباق
    double price;       // سعر الأوردر
    int distance;      // مسافة التوصيل

public:
    // الـ Constructor عشان ندي قيم للأوردر أول ما يتخلق
    Order(int id, ORD_TYPE t, int AT) : ID(id), type(t), ArrivalTime(AT), size(0), price(0.0), distance(0) {}

    // دالة بترجع الـ ID لو احتجناه في مكان تاني
    int getID() const { return ID; }
    int getArrivalTime() const { return ArrivalTime; }
    int getSize() const { return size; }
    double getPrice() const { return price; }
    int getDistance() const { return distance; }

    // الجزء السحري: ده اللي بيخلي الـ cout تطبع الـ ID على طول لما نديها الأوردر
    friend std::ostream& operator<<(std::ostream& os, const Order* ord) {
        if (ord) os << ord->ID;
        return os;
    }
};