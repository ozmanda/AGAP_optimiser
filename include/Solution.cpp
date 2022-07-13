//
// Created by julia on 05/04/2022.
//

#include "Solution.h"
#include "Flight.h"
#include "vector"
#include "numeric"
#include "cmath"
#include "iostream"
using namespace std;


Solution::Solution() {
}

Solution::Solution(vector<gate> * gatesp, vector<Flight> * flightsp) {
    gatesp = gatesp;
    flightsp = flightsp;
    assignment = vector_matrix(gatesp->size(), flightsp->size());
}


void Solution::set_objective_functions() {
    calculate_gate_idle_variance();
    nflights_assigned_to_apron();
}


void Solution::calculate_gate_idle_variance() {
    vector<double> timeDiffs;
    for (int flightIndex = 0; flightIndex < flightsp->size(); ++flightIndex) {
        long long timediff = 100000;
        for (int gateIndex = 0; gateIndex < gatesp->size(); ++gateIndex) {
            vector<vector<int>> assignment_view = assignment;
            if (assignment[gateIndex][flightIndex] != 1) {continue;}
            for (int priorFlightIndex = 0; priorFlightIndex < flightsp->size(); ++priorFlightIndex) {
                if (assignment[gateIndex][flightIndex] != 1 and
                    (*flightsp)[flightIndex].arrS > (*flightsp)[priorFlightIndex].depS and
                    (*flightsp)[flightIndex].arrS - (*flightsp)[priorFlightIndex].depS < timediff){
                    timeDiffs.push_back((*flightsp)[flightIndex].arrS - (*flightsp)[priorFlightIndex].depS);
                }
            }
        }
    }
    double mean = accumulate(timeDiffs.begin(), timeDiffs.end(), 0.0) / timeDiffs.size();
    double sq_sum = inner_product(timeDiffs.begin(), timeDiffs.end(), timeDiffs.begin(), 0.0);
    double slackTimeVariance = sqrt(sq_sum / timeDiffs.size() - mean * mean);

    // assign calculated variance to gateVariance
    gateVariance = slackTimeVariance;
}


void Solution::nflights_assigned_to_apron() {
    int counter = 0;
    for (int flightIndex = 0; flightIndex < flightsp->size(); ++flightIndex) {
        if (assignment[0][flightIndex] == 1){
            ++counter;
        }
    }
    apronAssignments = counter;
}

