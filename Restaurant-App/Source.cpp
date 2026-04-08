#include <iostream>
#include "Restaurant.h"
#include "UI.h"
#include <ctime>

using namespace std;

int main() {
    srand((unsigned int)time(0)); 

    Restaurant* myRestaurant = new Restaurant();
    UI* myUI = new UI(MODE_INTR);

    myRestaurant->RandomSimulation(myUI);

    delete myRestaurant;
    delete myUI;
    return 0;
}