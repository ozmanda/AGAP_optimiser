//
// Created by julia on 05/04/2022.
//

#ifndef OPTIMISER_SOLUTION_H
#define OPTIMISER_SOLUTION_H

#include "vector"
using namespace std;

class Solution {
public:
    // assignment matrix: vector of length (nFlights) containing vectors of length (nGates)
    vector<vector<int>> assignment;
    vector<float> values;
    float energy;
};


#endif //OPTIMISER_SOLUTION_H
