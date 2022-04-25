//
// Created by julia on 05/04/2022.
//

#include "SA.h"
#include "iostream"
#include "../src/utils.h"
using namespace std;

// CLASS CONSTRUCTOR
SA::SA(vector<gate> g, vector<Flight> f, string * gateConflictPath):nGates(g.size()), nFlights(f.size()) {
    gates = g;
    flights = f;
    emptyAssignment = vector_matrix(nGates, nFlights);
    flightConflicts = find_flight_conflicts(flights);
    gateConflicts = convert_to_bool(parse_file(gateConflictPath), nGates, nGates);
    currentSolution =  greedy();
    temperature = calculate_initial_temperature();
    epoch = Epoch(temperature, currentSolution);
}

// Public functions used outside of the calss

void SA::run_optimiser() {
    // run optimiser while the evaluate_termination() function does NOT return true
    while (not evaluate_termination()){
        currentSolution = epoch.run_epoch();
    }
}

// FUNCTIONS FOR PRIVATE CALCULATIONS
Solution SA::greedy() {
    // create new instance of Solution to put results in and fill assignment
    Solution greedySolution;
    greedySolution.assignment = emptyAssignment;
    bool assigned;
    for (int flight = 0; flight < nFlights; ++flight) {
        assigned = false;
        for (int gate = 0; gate < nGates; ++gate) {
            if (gate_availability(&greedySolution, flight, gate)){
                greedySolution.assignment[gate][flight] = 1;
                assigned = true;
            } else continue;
        }
        if (not assigned){
            cout << "Flight with registration number " << flights[flight].reg << " arriving at " <<
            flights[flight].arr.tm_hour << ":" << flights[flight].arr.tm_min << " could not be assigned" <<
            "(NO GATE AVAILABLE)" << endl;
        }
    }
    return greedySolution;
}



float SA::calculate_initial_temperature() {
    return 0;
}


vector<vector<bool>> SA::find_flight_conflicts(vector<Flight> flights) const {
    vector<vector<bool>> conflictingFlights = vector_matrix_bool(nFlights, nFlights);
    for (int i = 0; i < nFlights; ++i) {
        vector<bool> row;
        for (int j = 0; j < nFlights; ++j) {
            if (i != j){
                if (flights[i].arrS > flights[j].depS or flights[i].depS < flights[j].arrS){
                    row.push_back(false);
                }
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
    if (gates[gateIndex].body.size() != 2 and gates[gateIndex].body[0] != flights[flightIndex].body){
        return false;
    }
    // for each conflicting flight ...
    for (int conflictFlight = 0; conflictFlight < nFlights; ++conflictFlight) {
        if (flightIndex == conflictFlight) { continue; }
        if (flightConflicts[flightIndex][conflictFlight]){
            // evaluate if it assigned to this gate
            if (greedySolution->assignment[gateIndex][flightIndex] == 1){
                return false; break;
            }
            // evaluate if it assigned to a conflicting gate
            for (int conflictGate = 0; conflictGate < nGates; ++conflictGate) {
                if (gateConflicts[gateIndex][conflictGate] and greedySolution->assignment[conflictGate][conflictFlight] == 1){
                    return false; break;
                }
            }
        }
    }
    return true;
}


bool SA::evaluate_termination() {
    return false;
}




