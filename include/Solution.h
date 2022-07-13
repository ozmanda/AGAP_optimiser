//
// Created by julia on 05/04/2022.
//

#ifndef OPTIMISER_SOLUTION_H
#define OPTIMISER_SOLUTION_H

#include "vector"
#include "../src/utils.h"
#include "Flight.h"

class Solution {
public:
    // Functions which need to be accessed from outside the class
    Solution();
    Solution(vector<gate> * gatesp, vector<Flight> * flightsp);
    void set_objective_functions();

    // Values which need to be accessed from outside the class
    vector<vector<int>> assignment;
    double gateVariance;
    int apronAssignments;
    // pointers to values outside of the class
    vector<gate> * gatesp;
    vector<Flight> * flightsp;

private:
    // functions only required within the class (objective function value calculation
    void calculate_gate_idle_variance();
    void nflights_assigned_to_apron();
};


#endif //OPTIMISER_SOLUTION_H
