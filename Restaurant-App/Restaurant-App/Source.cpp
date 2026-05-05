#include <iostream>
#include <fstream>
#include "Restaurant.h"
#include "UI.h"
using namespace std;

int main() {
    Restaurant* r  = new Restaurant();
    UI*         ui = new UI(MODE_INTR);

    string inputFile, outputFile;
    cout << "Enter input file name:  "; cin >> inputFile;
    cout << "Enter output file name: "; cin >> outputFile;

    // Validate input file exists
    {
        ifstream test(inputFile);
        if (!test.is_open()) {
            cout << "\nERROR: Could not open \"" << inputFile << "\".\n";
            cout << "Enter full path to input file: ";
            cin >> inputFile;
            ifstream test2(inputFile);
            if (!test2.is_open()) {
                cout << "Still could not open file. Exiting.\n";
                delete r; delete ui; return 1;
            }
        }
    }

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
