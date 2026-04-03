#include "Restaurant.h"
#include "UI.h"
#include "RequestAction.h"

int main()
{
    Restaurant R;
    UI ui(MODE_INTR);

    // Connect UI to Restaurant
    // (you already pass it in RunSimulation)

    // ===== Add Dummy Actions =====
    R.AddAction(new RequestAction(0, &R, TYPE_ODN, 1));
    R.AddAction(new RequestAction(1, &R, TYPE_ODN, 2));
    R.AddAction(new RequestAction(2, &R, TYPE_ODN, 3));

    // ===== Run Simulation =====
    R.RunSimulation(&ui);

    return 0;
}


















/*
#include <iostream>
#include "Order.h"
#include "include/LinkedQueue.h"

using namespace std;

int main() {

    LinkedQueue<Order*> pendingOrders;


    Order* o1 = new Order(105, TYPE_NRM, 1);
    Order* o2 = new Order(210, TYPE_VIP, 2);


    pendingOrders.enqueue(o1);
    pendingOrders.enqueue(o2);


    cout << "Testing Print Function: ";
    pendingOrders.print();

    cout << "\nTesting Count Function: ";
    cout << pendingOrders.getCount() << endl;

    return 0;
}



*/