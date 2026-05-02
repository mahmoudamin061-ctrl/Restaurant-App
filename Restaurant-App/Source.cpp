#include <iostream>
#include <fstream>
#include "Restaurant.h"
#include "UI.h"
using namespace std;

int main() {
    Restaurant* r = new Restaurant();
    UI* ui = new UI(MODE_INTR);

    string inputFile, outputFile;
    cout << "Enter input file name:  "; cin >> inputFile;
    cout << "Enter output file name: "; cin >> outputFile;

    // Validate the file exists before proceeding
    {
        ifstream test(inputFile);
        if (!test.is_open()) {
            cout << "\nERROR: Could not open \"" << inputFile << "\".\n";
            cout << "Tip: Place input.txt next to the .exe, OR enter the full path.\n";
            cout << "Enter full path to input file: ";
            cin >> inputFile;
            test.close();
            ifstream test2(inputFile);
            if (!test2.is_open()) {
                cout << "Still could not open file. Exiting.\n";
                delete r; delete ui; return 1;
            }
        }
    }

    // MUST load before RunSimulation
    r->LoadFromFile(inputFile);

    // Mode selection comes after load so data is ready
    ui->SelectMode();

    r->RunSimulation(ui);
    r->SaveToFile(outputFile);

    if (ui->GetMode() == MODE_SILENT)
        cout << "Simulation ends. Output written to: " << outputFile << "\n";

    delete r;
    delete ui;
    return 0;
}
