//
// Created by julia on 12/04/2022.
//

#ifndef OPTIMISER_UTILS_H
#define OPTIMISER_UTILS_H

#include "string"
#include "../include/Flight.h"
using namespace std;

// data structures
struct gate{
    string name;
    vector<string> body;
    int preferenceValue;
    float distance;
};

// Data loading functions
vector<vector<string>> parse_file(string * filename);
vector<string> comma_separate(string * line);
struct tm time_parser(string datetime);
vector<Flight> flightData(string * flightplanpathp);
vector<gate> gateData(string * gatepathp);
vector<vector<int>> gateConflicts(string * gateconflictsp);

// empty vector and matrix functions
vector<vector<int>> vector_matrix(int x, int y);
vector<vector<bool>> vector_matrix_bool(int x, int y);

// converters
vector<vector<bool>> convert_to_bool(vector<vector<string>> original, int xdim, int ydim);

// random number sampling
int random_number(int min, unsigned long long max);


#endif //OPTIMISER_UTILS_H
