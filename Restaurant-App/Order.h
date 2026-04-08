#pragma once
#include <iostream>

// أنواع الأوردرات المتاحة في المطعم
enum ORD_TYPE { TYPE_NRM, TYPE_VGAN, TYPE_VIP, TYPE_ODG, TYPE_ODN, TYPE_OT, TYPE_OVG, TYPE_OVC, TYPE_OVN };
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

    int getID() const { return ID; }
    ORD_TYPE getType() const { return type; }
    int getArrivalTime() const { return ArrivalTime; }
    int getSize() const { return size; }
    double getPrice() const { return price; }
    int getDistance() const { return distance; }

    // 👇 التعديل السحري هنا: دالة الطباعة دي هتحط الأقواس أوتوماتيك 👇
    friend std::ostream& operator<<(std::ostream& os, const Order* ord) {
        if (ord) {
            // لو الأوردر Normal 
            if (ord->type == TYPE_NRM || ord->type == TYPE_ODN || ord->type == TYPE_OVN) {
                os << "[" << ord->ID << "]";
            }
            // لو الأوردر Vegan
            else if (ord->type == TYPE_VGAN) {
                os << "(" << ord->ID << ")";
            }
            // لو الأوردر VIP أو الأنواع التانية اللي محتاجة أقواس
            else if (ord->type == TYPE_VIP || ord->type == TYPE_OVG || ord->type == TYPE_OVC || ord->type == TYPE_ODG) {
                os << "{" << ord->ID << "}";
            }
            // لو أي نوع تاني مش معروف
            else {
                os << ord->ID;
            }
        }
        return os;
    }
};