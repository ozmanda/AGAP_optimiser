//
// Created by julia on 05/04/2022.
//

#include "SA.h"
#include "iostream"
#include "Epoch.h"
#include "../src/utils.h"
#include "exception"
using namespace std;

// CLASS CONSTRUCTOR
SA::SA(vector<gate> * g, vector<Flight> * f, string * gateConflictPath):nGates(g->size()), nFlights(f->size()) {
    gates = g;
    flights = f;
    flightConflicts = find_flight_conflicts(flights);
    gateConflicts = convert_to_bool(parse_file(gateConflictPath), nGates, nGates);

    // initialise empty solution and run greedy algorithm for initial solution
    currentSolution = Solution(g, f);
    greedy();

    // calculate initial temperature
    temperature = calculate_initial_temperature();

    // initialise epoch
    epoch = Epoch(&temperature, &currentSolution, flights, gates, &gateConflicts, &flightConflicts);
}

// Public functions used outside of the calss

void SA::run_optimiser() {
    // run optimiser while the evaluate_termination() function does NOT return true
    bool termination = false;
    while (not termination){
        currentSolution = epoch.run_epoch();
        if (epoch.acceptedSolutions == 0){
            termination = true;
            archive = epoch.archive;
        }
    }
}


// FUNCTIONS FOR PRIVATE CALCULATIONS
Solution SA::greedy_solution() {
    // create new instance of Solution to put results in and fill assignment
//    vector<vector<bool>> conflicts = gateConflicts;
    Solution greedySolution;
//    greedySolution.assignment = emptyAssignment;
    bool assigned;
    for (int flightIndex = 0; flightIndex < nFlights; ++flightIndex) {
        assigned = false;
        for (int gateIndex = 1; gateIndex < nGates; ++gateIndex) {
            if (gate_availability(&greedySolution, flightIndex, gateIndex)){
                greedySolution.assignment[gateIndex][flightIndex] = 1;
                assigned = true;
                break;
            }
        }
        if (not assigned){
            cout << "Flight with registration number " << (*flights)[flightIndex].reg << " arriving at " <<
                 (*flights)[flightIndex].arr.tm_hour << ":" << (*flights)[flightIndex].arr.tm_min << " could not be assigned" <<
                 "(NO GATE AVAILABLE)" << endl;
        }
    }
    return greedySolution;
}

void SA::greedy(){
    bool assigned;
    for (int flightIndex = 0; flightIndex < nFlights; ++flightIndex) {
        assigned = false;
        for (int gateIndex = 1; gateIndex < nGates; ++gateIndex) {
            if (gate_availability(&currentSolution, flightIndex, gateIndex)){
                currentSolution.assignment[gateIndex][flightIndex] = 1;
                assigned = true;
                break;
            }
        }
        if (not assigned){
            cout << "Flight with registration number " << (*flights)[flightIndex].reg << " arriving at " <<
                 (*flights)[flightIndex].arr.tm_hour << ":" << (*flights)[flightIndex].arr.tm_min << " could not be assigned" <<
                 "(NO GATE AVAILABLE)" << endl;
        }
    }
}



float SA::calculate_initial_temperature() {
    return 0;
}


vector<vector<bool>> SA::find_flight_conflicts(vector<Flight> * flights) const {
    vector<vector<bool>> conflictingFlights;
    conflictingFlights.reserve(nFlights);
    for (int i = 0; i < nFlights; ++i) {
        vector<bool> row;
        row.reserve(nFlights);
        for (int j = 0; j < nFlights; ++j) {
            if (i != j){
                if ((*flights)[i].arrS > (*flights)[j].depS or (*flights)[i].depS < (*flights)[j].arrS){
                    row.push_back(false);
                } else  {
                    row.push_back(true);
                }
            } else {
                row.push_back(true);
            }
        }
        conflictingFlights.push_back(row);
    }
    return conflictingFlights;
}

bool SA::gate_availability(Solution * greedySolution, int flightIndex, int gateIndex) {
    // true until proven unable to host the A/C
    bool available = true;
    // either the gate can handle both NB and WB or it has one size which must == that of the A/C
    if ((*gates)[gateIndex].body.size() != 2 and (*gates)[gateIndex].body[0] != (*flights)[flightIndex].body){
        return false;
    }
    // for each conflicting flight ...
    for (int conflictFlight = 0; conflictFlight < nFlights; ++conflictFlight) {
        if (flightIndex == conflictFlight) { continue; }
        if (flightConflicts[flightIndex][conflictFlight]){
            // evaluate if it assigned to this gate
            if (greedySolution->assignment[gateIndex][conflictFlight] == 1){
                return false;
            }
            // evaluate if it assigned to a conflicting gate
            for (int conflictGate = 0; conflictGate < nGates; ++conflictGate) {
                if (gateConflicts[gateIndex][conflictGate] and greedySolution->assignment[conflictGate][conflictFlight] == 1){
                    return false;
                }
            }
        }
    }
    return true;
}






