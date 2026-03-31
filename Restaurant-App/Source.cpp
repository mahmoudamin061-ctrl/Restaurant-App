#include <iostream>
#include "Order.h"        // ضفنا كلمة include/ قبل اسم الملف
#include "include/LinkedQueue.h"  // ضفنا كلمة include/ قبل اسم الملف

using namespace std;

int main() {
    // 1. نعمل طابور جديد بيشيل أوردرات
    LinkedQueue<Order*> pendingOrders;

    // 2. نعمل أوردرين للتجربة
    Order* o1 = new Order(105, TYPE_NRM, 1);
    Order* o2 = new Order(210, TYPE_VIP, 2);

    // 3. ندخلهم الطابور
    pendingOrders.enqueue(o1);
    pendingOrders.enqueue(o2);

    // 4. نختبر الشغل بتاعك كـ Person 1
    cout << "Testing Print Function: ";
    pendingOrders.print(); // المفروض تطبع: 105, 210

    cout << "\nTesting Count Function: ";
    cout << pendingOrders.getCount() << endl; // المفروض تطبع: 2

    return 0;
}