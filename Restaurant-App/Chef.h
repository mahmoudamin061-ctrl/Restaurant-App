#pragma once
#include <iostream>

// أنواع الشيفات المتاحة في المطعم
enum CHEF_TYPE { CHEF_NRM, CHEF_VGAN, CHEF_VIP };

class Chef {
private:
    int ID;             // رقم الشيف
    CHEF_TYPE type;     // تخصص الشيف (عادي، نباتي، VIP)
    int speed;          // سرعة الشيف في تحضير الأطباق

public:
    // الـ Constructor
    Chef(int id, CHEF_TYPE t, int s) : ID(id), type(t), speed(s) {}

    // دالة بترجع الـ ID
    int getID() const { return ID; }

    // الـ Overloading عشان الـ cout تطبع رقم الشيف
    friend std::ostream& operator<<(std::ostream& os, const Chef* c) {
        if (c) os << c->ID;
        return os;
    }
};