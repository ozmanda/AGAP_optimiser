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

class SA {

private:
    float temp;
    Archive archive;
    Epoch epoch;
    vector<Flight> flights;

    // Functions
    float init_temp();
    float update_temp();
    Solution greedy();

//    Solution candidate_solution;
//    Solution current_solution;
};


#endif //OPTIMISER_SA_H
