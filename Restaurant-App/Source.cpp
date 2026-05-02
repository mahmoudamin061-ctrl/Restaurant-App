#include <iostream>
#include "Restaurant.h"
#include "UI.h"

using namespace std;

int main() {
    Restaurant* r = new Restaurant();
    UI* ui = new UI(MODE_INTR);

    r->LoadFromFile("input.txt");
    r->RunSimulation(ui);
    r->SaveToFile("output.txt");

    delete r;
    delete ui;
    return 0;
}