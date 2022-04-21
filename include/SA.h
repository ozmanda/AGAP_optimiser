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
private:
    float calculate_initial_temperature();
    vector<vector<bool>> flightConflicts;
    vector<vector<bool>> gateConflicts;
    Archive archive;
    float temperature;
    Epoch epoch;
    vector<gate> gates;
    vector<Flight> flights;
    vector<vector<int>> emptyAssignment;

    const int nGates, nFlights;
    // functions for private calculations
    vector<vector<bool>> find_flight_conflicts(vector<Flight> flights) const;
    Solution greedy();
public:

    SA(vector<gate> gates, vector<Flight> flights, string * gateConflictPath);
    // Functions to set values from outside the class
    void set_initial_solution();
    void set_initial_temperature();
    void update_temperature();

    Solution candidate_solution;
    Solution current_solution;
};


#endif //OPTIMISER_SA_H
