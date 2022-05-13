//
// Created by julia on 05/04/2022.
//

#include "Solution.h"
#include "Flight.h"
#include "vector"
#include "numeric"
#include "cmath"
using namespace std;


Solution::Solution() {
}

Solution::Solution(vector<gate> * gatespointer, vector<Flight> * flightspointer) {
    gatesp = gatespointer;
    flightsp = flightspointer;
}


void Solution::set_objective_functions() {
    calculate_gate_idle_variance();
    nflights_assigned_to_apron();
}


void Solution::calculate_gate_idle_variance() {
    vector<double> timeDiffs;
    int previousFlightIndex = -1;
    // THOUGHTS: flights MUST be ordered by arrival time and no invalid assignments are generated / allowed
    // SO: iterating through flights looking for assignment[gate][flight] == 1 will give first flight
    // THEN: each subsequent flight will be directly after the previous flight with assigment[gate][flight] == 1
    for (int gateIndex = 0; gateIndex < gatesp->size(); ++gateIndex) {
        long long dT = 0;
        int counter = 0;
        for (int flightIndex = 0; flightIndex < flightsp->size(); ++flightIndex) {
            if (assignment[gateIndex][flightIndex] == 1){
                ++counter;
                // initialise with the first flight (no slack time can be calculated)
                if (previousFlightIndex == -1){
                    previousFlightIndex = flightIndex;
                    continue;
                } else {
                    dT += (*flightsp)[flightIndex].arrS - (*flightsp)[previousFlightIndex].depS;
                    previousFlightIndex = flightIndex;
                }
            }
        }
        timeDiffs.push_back(dT/counter);
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

