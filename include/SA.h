//
// Created by julia on 05/04/2022.
//

#ifndef OPTIMISER_SA_H
#define OPTIMISER_SA_H

#include "vector"
#include "Archive.h"
#include "Epoch.h"
#include "Solution.h"
#include "Flight.h"
#include "../src/utils.h"

class SA {
public:
    // Constructor
    SA(vector<gate> * gates, vector<Flight> * flights, string * gateConflictPath);

    // Functions to be used from outside the class
    void run_optimiser();

    // Variables which must be visible from outside the class
    Solution currentSolution;
    vector<gate> * gates;
    vector<Flight> * flights;
    Archive archive;
private:
    // Private variables
    const int nGates, nFlights;
    vector<vector<bool>> flightConflicts;
    vector<vector<bool>> gateConflicts;
    float temperature;
    Epoch epoch;

    // functions for private calculations
    vector<vector<bool>> find_flight_conflicts(vector<Flight> * flights) const;
    void greedy();
    float calculate_initial_temperature();
    bool gate_availability(Solution * greedySolution, int flightIndex, int gateIndex);

};


#endif //OPTIMISER_SA_H