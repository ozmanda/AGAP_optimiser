#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include "../include/Flight.h"
#include "../include/SA.h"
#include "../src/utils.h"
using namespace std;


int main() {
    // absolute paths to gate data and flightplan data
    string gatepath = "C:/Users/julia/OneDrive - ZHAW/MSc Engineering/VT2_AGAP/Data/Test_Gates.csv";
    string gateconflictpath = "C:/Users/julia/OneDrive - ZHAW/MSc Engineering/VT2_AGAP/Data/Test_Gate_Conflicts.csv";
    string flightplanpath = "C:/Users/julia/OneDrive - ZHAW/MSc Engineering/VT2_AGAP/Data/Flightplans/Test_Dataset_1.csv";

    // load gate and flight data into vectors
    vector<gate> gates = gateData(&gatepath);
    vector<Flight> flights = flightData(&flightplanpath);

    // initialise SA optimiser with gates and flights
    SA optimiser(gates, flights, &gateconflictpath);
    SA

    return 0;
}


