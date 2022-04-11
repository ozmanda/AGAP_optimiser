//
// Created by julia on 05/04/2022.
//

#ifndef OPTIMISER_SOLUTION_H
#define OPTIMISER_SOLUTION_H

#include "vector"
using namespace std;

class Solution {
    vector<vector<int>> assignment;
    vector<float> values;
    float energy;
    Solution generate_neighbour();
};


#endif //OPTIMISER_SOLUTION_H
