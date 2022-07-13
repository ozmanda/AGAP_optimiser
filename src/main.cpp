#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include "../include/SA.h"
using namespace std;


int main() {
    // absolute paths to gate data and flightplan data
    string gatepath = "C:/Users/julia/OneDrive - ZHAW/MSc Engineering/VT2_AGAP/Data/Test_Gates.csv";
    string gateconflictpath = "C:/Users/julia/OneDrive - ZHAW/MSc Engineering/VT2_AGAP/Data/Test_Gate_Conflicts.csv";
    string flightplanpath = "C:/Users/julia/OneDrive - ZHAW/MSc Engineering/VT2_AGAP/Data/Flightplans/Test_Dataset_6.csv";

    // load gate, gate conflicts and flight data into vectors
    vector<gate> gates = gateData(&gatepath);
    vector<Flight> flights = flightData(&flightplanpath);

    // initialise and run the SA optimiser
    SA optimiser = SA(&gates, &flights, &gateconflictpath);
    optimiser.run_optimiser();

    return 0;
}

