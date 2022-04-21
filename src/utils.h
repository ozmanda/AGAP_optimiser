//
// Created by julia on 12/04/2022.
//

#ifndef OPTIMISER_UTILS_H
#define OPTIMISER_UTILS_H

vector<vector<string>> parse_file(string * filename);
struct gate{
    string name;
    vector<string> conflicts;
    vector<string> body;
    int preferenceValue;
};

vector<string> comma_separate(string * line);

vector<vector<int>> vector_matrix(int x, int y);
vector<vector<bool>> vector_matrix_bool(int x, int y);

vector<vector<bool>> convert_to_bool(vector<vector<string>> original, int xdim, int ydim);

#endif //OPTIMISER_UTILS_H
