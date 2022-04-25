//
// Created by julia on 05/04/2022.
//

#ifndef OPTIMISER_EPOCH_H
#define OPTIMISER_EPOCH_H

#include "vector"
#include "Solution.h"
#include "Archive.h"
#include "Flight.h"
#include "../src/utils.h"

class Epoch {
public:
    Epoch(float * initialTemperaturep, Solution * initialSolutionp, vector<Flight> * flights, vector<gate> * gates,
          vector<vector<int>> * gateConflicts, vector<vector<int>> * flightConflicts);
    Solution run_epoch();
private:
    // private variables used during the epoch
    Solution currentSolution, candidateSolution;
    Archive frontEstimation;
    float temperature;
    struct interval {
        time_t t1, t2, t3, t4;
        vector<int> flightIndices;
        int nextFlightIndex, previousFlightIndex, currentGateIndex = -1;
    };

    // pointers to data outside of class
    vector<Flight> * flightsp;
    vector<gate> * gatesp;
    vector<vector<int>> * gateConflictsp;
    vector<vector<int>> * flightConflictsp;

    // private functions for running an epoch
    bool evaluate_termination();
    Solution generate_neighbour();
    void insert_move();
    void interval_exchange_move();
    void interval_next(interval * flightInterval);
    void interval_previous(interval * flightInterval);
    bool interval_compatible(interval * A, interval * B);
    void apron_exchange_move();
    int sample_conflicting_flight(int flightIndex);
    bool gate_availability(int flightIndex, int gateIndex);


};


#endif //OPTIMISER_EPOCH_H
