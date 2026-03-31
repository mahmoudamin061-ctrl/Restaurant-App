#pragma once
#include <iostream>

// حالة الطاولة (فاضية ولا عليها زباين)
enum TABLE_STATUS { EMPTY, OCCUPIED };

class Table {
private:
    int ID;                 // رقم الطاولة
    int capacity;           // بتشيل كام شخص
    TABLE_STATUS status;    // حالتها الحالية

public:
    // الـ Constructor
    Table(int id, int cap) : ID(id), capacity(cap), status(EMPTY) {}

    // دالة بترجع الـ ID
    int getID() const { return ID; }

    // الأهم: الـ Overloading عشان الـ cout تطبع رقم الطاولة
    friend std::ostream& operator<<(std::ostream& os, const Table* t) {
        if (t) os << t->ID;
        return os;
    }
};