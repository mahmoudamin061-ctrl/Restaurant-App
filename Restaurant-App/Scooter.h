#pragma once
#include <iostream>

// حالة السكوتر (واقف في المطعم ولا بيوصل أوردر)
enum SCOOTER_STATUS { IDLE, DELIVERING };

class Scooter {
private:
    int ID;                 // رقم السكوتر
    int speed;              // سرعة التوصيل
    SCOOTER_STATUS status;  // حالته الحالية

public:
    // الـ Constructor
    Scooter(int id, int s) : ID(id), speed(s), status(IDLE) {}

    // دالة بترجع الـ ID
    int getID() const { return ID; }

    // الأهم: الـ Overloading عشان الـ cout تطبع رقم السكوتر وهو في الـ Queue
    friend std::ostream& operator<<(std::ostream& os, const Scooter* s) {
        if (s) os << s->ID;
        return os;
    }
};