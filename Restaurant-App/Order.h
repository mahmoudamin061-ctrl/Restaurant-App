#pragma once
#include <iostream>

// أنواع الأوردرات المتاحة في المطعم
enum ORD_TYPE { TYPE_NRM, TYPE_VGAN, TYPE_VIP };

class Order {
private:
    int ID;             // رقم الأوردر
    ORD_TYPE type;      // نوع الأوردر (عادي، نباتي، VIP)
    int ArrivalTime;    // وقت وصول الأوردر

public:
    // الـ Constructor عشان ندي قيم للأوردر أول ما يتخلق
    Order(int id, ORD_TYPE t, int AT) : ID(id), type(t), ArrivalTime(AT) {}

    // دالة بترجع الـ ID لو احتجناه في مكان تاني
    int getID() const { return ID; }

    // الجزء السحري: ده اللي بيخلي الـ cout تطبع الـ ID على طول لما نديها الأوردر
    friend std::ostream& operator<<(std::ostream& os, const Order* ord) {
        if (ord) os << ord->ID;
        return os;
    }
};