/*
* This is the driver main for the dialog section of the engine 
*/

#include <iostream>

#include "dialog.h"


using namespace std;

int main() {
    dialog* dialog_test = new dialog();
    dialog_test->init();


    cout << "ended test of the dialog engine" << endl;
    while (true);
    return 0;
}
