//
// Created by julia on 12/04/2022.
//

#include "vector"
#include "string"
#include "iostream"
#include "sstream"
#include "fstream"
using namespace std;

vector<vector<string>> parse_file(string * filename){
    // create array "content", vector "row" and the "line" and "word" variables
    vector<vector<string>> content;
    vector<string> row;
    string line, word;

    // open file at flightplanpath and fill content
    fstream file (*filename, ios::in);
    if(file.is_open()){
        while(getline(file, line)){
            row.clear();
            stringstream str(line);

            while(getline(str, word, ';')){
                row.push_back(word);
            }
            content.push_back(row);
        }
    }
    else
        cout << "Could not open file at location" << *filename << endl;

    return content;
};


vector<vector<bool>> convert_to_bool(vector<vector<string>> original, const int xdim, const int ydim){
    vector<vector<bool>> converted;
    for (int i = 0; i < xdim; ++i) {
        vector<bool> row;
        for (int j = 0; j < ydim; ++j) {
            if (stoi(original[i][j]) == 1){
                row.push_back(true);
            } else row.push_back(false);
        }
    }
    return converted;
}


vector<string> comma_separate(string * line){
    stringstream ss(*line);
    vector<string> result;

    while( ss.good() ){
        string substr;
        getline(ss, substr, ',');
        result.push_back(substr);
    };
    result.shrink_to_fit();
    return result;
};


vector<vector<int>> vector_matrix(int x, int y){
    vector<vector<int>> vectorMatrix;
    for (int i = 0; i < x; ++i) {
        vector<int> gateAssignment;
        for (int j = 0; j < y; ++j) {
            gateAssignment.push_back(0);
        }
        gateAssignment.shrink_to_fit();
        vectorMatrix.push_back(gateAssignment);
    }
    vectorMatrix.shrink_to_fit();
    return vectorMatrix;
};

vector<vector<bool>> vector_matrix_bool(int x, int y){
    vector<vector<bool>> vectorMatrix;
    for (int i = 0; i < x; ++i) {
        vector<bool> gateAssignment;
        for (int j = 0; j < y; ++j) {
            gateAssignment.push_back(true);
        }
        gateAssignment.shrink_to_fit();
        vectorMatrix.push_back(gateAssignment);
    }
    vectorMatrix.shrink_to_fit();
    return vectorMatrix;
};
