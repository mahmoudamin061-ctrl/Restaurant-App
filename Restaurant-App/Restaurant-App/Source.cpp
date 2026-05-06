#include <iostream>
#include "Restaurant.h"
#include "UI.h"
using namespace std;

int main() {
    //srand((unsigned int)time(0)); 
    Restaurant* r  = new Restaurant();
    UI*         ui = new UI(MODE_INTR);
    //myRestaurant->RandomSimulation(myUI);
    string inputFile, outputFile;
    cout << "Enter input file name:  "; cin >> inputFile;

    cout << "Enter output file name: "; cin >> outputFile;

   \

    r->LoadFromFile(inputFile);
    ui->SelectMode();
    r->RunSimulation(ui);
    r->SaveToFile(outputFile);

    if (ui->GetMode() == MODE_SILENT)
        cout << "Simulation ends. Output written to: " << outputFile << "\n";

    delete r;
    delete ui;
    return 0;
}
