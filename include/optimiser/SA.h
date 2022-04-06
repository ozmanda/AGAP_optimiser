//
// Created by julia on 05/04/2022.
//

#ifndef OPTIMISER_SA_H
#define OPTIMISER_SA_H

#include "Archive.h"
#include "Epoch.h"
#include "Solution.h"

class SA {

private:
    float init_temp;
    float temperature;

    Archive archive;
    Epoch epoch;
    Solution current_solution;
    Solution candidate_solution;
};


#endif //OPTIMISER_SA_H
