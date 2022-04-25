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
    SA(vector<gate> gates, vector<Flight> flights, string * gateConflictPath);
    // Functions to be used from outside the class
    void run_optimiser();
    bool evaluate_termination();
    // Variables which must be visible from outside the class
    Solution currentSolution;
    vector<gate> gates;
    vector<Flight> flights;
private:
    const int nGates, nFlights;
    vector<vector<bool>> flightConflicts;
    vector<vector<bool>> gateConflicts;
    Archive archive;
    float temperature;
    Epoch epoch;
    vector<vector<int>> emptyAssignment;
    // functions for private calculations
    vector<vector<bool>> find_flight_conflicts(vector<Flight> flights) const;
    Solution greedy();
    float calculate_initial_temperature();
    bool gate_availability(Solution * greedySolution, int flightIndex, int gateIndex);
    bool algorithm_termination();

};


#endif //OPTIMISER_SA_H
