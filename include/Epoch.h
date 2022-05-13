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
    Epoch();
    Epoch(float *initialTemperaturep,
          Solution *initialSolutionp,
          vector<Flight> *flights,
          vector<gate> *gates,
          vector<vector<bool>> *gateConflicts,
          vector<vector<bool>> *flightConflicts);
    Solution run_epoch();
    int acceptedSolutions = 0;
    Archive archive;
private:
    // private variables used during the epoch
    Solution currentSolution, candidateSolution;
    int generatedSolutions = 0;
    vector<Solution> newNonDominated;
    float temperature{};
    struct interval {
        time_t t1, t2, t3, t4;
        vector<int> flightIndices;
        int nextFlightIndex, previousFlightIndex = -1;
        int currentGateIndex;
    };

    // pointers to data outside of class
    vector<Flight> * flightsp{};
    vector<gate> * gatesp{};
    vector<vector<bool>> * gateConflictsp{};
    vector<vector<bool>> * flightConflictsp{};

    // private functions for running an epoch
    void evaluate_acceptance();
    void generate_neighbour();
    void insert_move();
    void interval_exchange_move();
    void set_current_gate(interval * flightInterval);
    void next_flight(interval * flightInterval);
    void prior_flight(interval * flightInterval);
    bool interval_compatible(interval * A, interval * B);
    void apron_exchange_move();
    int sample_conflicting_flight(int flightIndex);
    bool gate_availability(int flightIndex, int gateIndex);
    float calculate_deltaE(vector<Solution> * FTildep, Solution * candidatep, Solution * currentp);


};


#endif //OPTIMISER_EPOCH_H
