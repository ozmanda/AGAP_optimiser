//
// Created by julia on 05/04/2022.
//

#include "Epoch.h"
#include "../src/utils.h"
#include "iostream"

// CONSTRUCTOR
Epoch::Epoch(float * initialTemperaturep, Solution * initialSolutionp, vector<Flight> * flights, vector<gate> * gates,
             vector<vector<int>> * gateConflicts, vector<vector<int>> * flightConflicts) {
    temperature = * initialTemperaturep;
    currentSolution = *initialSolutionp;
    candidateSolution = currentSolution;
    flightsp = flights;
    gatesp = gates;
    gateConflictsp = gateConflicts;
    flightConflictsp = flightConflicts;
}

// public functions called by SA to conduct an epoch
Solution Epoch::run_epoch() {
    // conduct one epoch of the SA algorithm and provide current best solution to SA optimiser
    while (not evaluate_termination()){
        // generate new solution
        candidateSolution = generate_neighbour();
        // evaluate acceptance

        // update archive


    }
}

bool Epoch::evaluate_termination() {
    return false;
}

Solution Epoch::generate_neighbour() {
    Solution neighbourSolution;
    bool solutiongenerated = false;
    while (not solutiongenerated){
        // choose one of three methods
        int randNum = random_number(1, 3);
        if (randNum == 1){
            neighbourSolution = insert_move();
            solutiongenerated = true;
        } else if (randNum == 2){
            neighbourSolution = interval_exchange_move();
            solutiongenerated = true;
        } else if (randNum == 3){
            neighbourSolution = apron_exchange_move();
            solutiongenerated = true;
        }
    }



    return neighbourSolution;
}

void Epoch::insert_move() {
    int flightIndex = random_number(0, flightsp->size());
    vector<int> availableGates;
    for (int gateIndex = 0; gateIndex < gatesp->size(); ++gateIndex) {
        if (gate_availability(flightIndex, gateIndex)){
            availableGates.push_back(gateIndex);
        }
    }
    if (availableGates.size() != 0){
        Solution insertSolution;
        int newGateIndex = random_number(0, availableGates.size());
        for (int gateIndex = 0; gateIndex < gatesp->size(); ++gateIndex) {
            if (gateIndex == newGateIndex){
                continue;
            } else if (currentSolution.assignment[gateIndex][flightIndex] == 1){
                candidateSolution.assignment[gateIndex][flightIndex] = 0;
                candidateSolution.assignment[newGateIndex][flightIndex] = 1;
                break;
            }
        }
    }
}

void Epoch::interval_exchange_move() {
    interval intervalA;
    intervalA.flightIndices.push_back(sample_flight());
    intervalA.t2 = (*flightsp)[intervalA.flightIndices[0]].arrS;
    intervalA.t3 = (*flightsp)[intervalA.flightIndices[0]].depS;

    interval intervalB;
    intervalB.flightIndices.push_back(sample_conflicting_flight(intervalA.flightIndices[0]));
    intervalB.t2 = (*flightsp)[intervalB.flightIndices[0]].arrS;
    intervalB.t3 = (*flightsp)[intervalB.flightIndices[0]].depS;

    bool compatible = interval_compatible(intervalA, intervalB);
    while (not compatible){
        bool success = false;
        if (intervalA.t2 < intervalB.t1){

        }

    }



}

bool Epoch::interval_compatible(interval * A, interval * B) {
    if (A->t2 < B->t1 or B->t2 < A->t1 or A->t3 > B->t4 or B->t3 > A->t4){
        return false;
    } else return true;
}

void Epoch::interval_next(interval * flightInterval) {
    if (flightInterval->currentGateIndex == -1){
        for (int gateIndex = 0; gateIndex < gatesp->size(); ++gateIndex) {
            if (currentSolution.assignment[gateIndex][flightInterval->flightIndices[0]] == 1) {
                flightInterval->currentGateIndex = gateIndex;
                break;
            }
        }
    }
    if (flightInterval->nextFlightIndex == -1){
        // determine next flight at the gate after the interval and adjust interval values
        long long timediff = 100000;
        for (int flightIndex = 0; flightIndex < flightsp->size(); ++flightIndex) {
            if (currentSolution.assignment[flightInterval->currentGateIndex][flightIndex] == 1 and
                (*flightsp)[flightIndex].arrS > flightInterval->t3 and
                (*flightsp)[flightIndex].arrS - flightInterval->t3 < timediff){
                    flightInterval->nextFlightIndex = flightIndex;
                    timediff = (*flightsp)[flightIndex].arrS - flightInterval->t3 < timediff;
            }
        }
        flightInterval->t4 = (*flightsp)[flightInterval->nextFlightIndex].arrS;
    } else {
        // add the index of the next flight to the interval and adjust t3
        flightInterval->flightIndices.push_back(flightInterval->nextFlightIndex);
        flightInterval->t3 = (*flightsp)[flightInterval->nextFlightIndex].depS;

        // determine the index of the next flight and adjust interval values
        long long timediff = 100000;
        for (int flightIndex = 0; flightIndex < flightsp->size(); ++flightIndex) {
            if (currentSolution.assignment[flightInterval->currentGateIndex][flightIndex] == 1 and
                (*flightsp)[flightIndex].arrS > (*flightsp)[flightInterval->nextFlightIndex].arrS and
                (*flightsp)[flightIndex].arrS - (*flightsp)[flightInterval->nextFlightIndex].arrS < timediff){
                flightInterval->nextFlightIndex = flightIndex;
                timediff = (*flightsp)[flightIndex].arrS - flightInterval->t3 < timediff;
            }
        }
        flightInterval->t4 = (*flightsp)[flightInterval->nextFlightIndex].arrS;
    }
}

void Epoch::interval_previous(interval * flightInterval) {
    
}

void Epoch::apron_exchange_move() {
    int flightIndex = sample_flight();
    int conflictingFlightIndex = sample_conflicting_flight(flightIndex);

}

int Epoch::sample_conflicting_flight(int flightIndex) {
    vector<int> conflictingFlights;
    // Find all conflicting flights
    for (int conflictingFlightIndex = 0; conflictingFlightIndex < flightsp->size(); ++conflictingFlightIndex) {
        if ((*flightConflictsp)[flightIndex][conflictingFlightIndex] == 1){
            conflictingFlights.push_back(conflictingFlightIndex);
        }
    }
    int conflictingFlightIndex = random_number(0, conflictingFlights.size());
    return conflictingFlightIndex;
}

bool Epoch::gate_availability(int flightIndex, int gateIndex) {
    // available until proven not to be
    bool available = true;

    // either the gate can handle both NB and WB or it has one size which must == that of the A/C
    if ((*gatesp)[gateIndex].body.size() != 2 and (*gatesp)[gateIndex].body[0] != (*flightsp)[flightIndex].body){
        return false;
    }
    // for each conflicting flight ...
    for (int conflictFlight = 0; conflictFlight < (*flightsp).size(); ++conflictFlight) {
        if (flightIndex == conflictFlight) { continue; }
        if ((*flightConflictsp)[flightIndex][conflictFlight]){
            // evaluate if it assigned to this gate
            if (currentSolution.assignment[gateIndex][flightIndex] == 1){
                return false; break;
            }
            // evaluate if it assigned to a conflicting gate
            for (int conflictGate = 0; conflictGate < (*gatesp).size(); ++conflictGate) {
                if ((*gateConflictsp)[gateIndex][conflictGate] and currentSolution.assignment[conflictGate][conflictFlight] == 1){
                    return false; break;
                }
            }
        }
    }
    return true;
}






