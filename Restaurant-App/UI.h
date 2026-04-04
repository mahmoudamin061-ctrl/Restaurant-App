#ifndef UI_H
#define UI_H

#include <iostream>
using namespace std;

class Restaurant;
class Order;

enum UI_MODE { MODE_INTR, MODE_SILENT };

class UI
{
private:
    UI_MODE mode;

public:
    UI(UI_MODE mode);

    void SelectMode();

    void PrintAll(Restaurant* R, int currentTimeStep);

    void PrintHeader(int timestep);

    void Wait();

    int GetMode() const;


    void PrintOrderFormat(Order* pOrd);
};

#endif