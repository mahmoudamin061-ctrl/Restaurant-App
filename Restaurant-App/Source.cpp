#include <iostream>
#include <ctime> 
#include "Restaurant.h"
#include "UI.h"

using namespace std;

int main() {

    srand(time(0));


    Restaurant* myRestaurant = new Restaurant();
    UI* myUI = new UI(MODE_INTR); 

    myUI->SelectMode();

    int currentTimeStep = 1;
    int orderIDCounter = 100; 


    while (currentTimeStep <= 50) {


        myRestaurant->GenerateRandomOrder(currentTimeStep, orderIDCounter);


        myRestaurant->ExecuteEvents(currentTimeStep);


        myUI->PrintAll(myRestaurant, currentTimeStep);

        currentTimeStep++; 
    }

    cout << "Simulation Ended.\n";


    delete myRestaurant;
    delete myUI;

    return 0;
}