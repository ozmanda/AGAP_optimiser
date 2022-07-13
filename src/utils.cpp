//
// Created by julia on 12/04/2022.
//

#include "vector"
#include "string"
#include "iostream"
#include "sstream"
#include "fstream"
#include "../include/Flight.h"
#include "utils.h"
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


vector<gate> gateData(string * gatepathp){
    vector<vector<string>> content = parse_file(gatepathp);
    vector<gate> gates;
    vector<string> bothbodies;
    // iterate through content and create gate struct to push back on the vector
    gate currentGate;

    // add the apron gate to the list of gates
    currentGate.name = "Apron";
    currentGate.body.push_back("NB");
    currentGate.body.push_back("WB");
    currentGate.preferenceValue = 100000;
    gates.push_back(currentGate);

    // iterate through the gates given in the file
    for (int i = 1; i < content.size(); ++i) {
        currentGate.name = content[i][0];
        currentGate.body = comma_separate(& content[i][2]);
        currentGate.preferenceValue = stoi(content[i][3]);
        gates.push_back(currentGate);
    }
    gates.shrink_to_fit();
    return gates;
};


vector<Flight> flightData(string * flightplanpathp){
    vector<vector<string>> content = parse_file(flightplanpathp);
    vector<Flight> flights;
    Flight currentFlight;

    for (int i = 1; i < content.size(); ++i) {
        currentFlight.arr = time_parser(content[i][0]);
        currentFlight.arrS = mktime(&currentFlight.arr);
        currentFlight.dep = time_parser(content[i][1]);
        currentFlight.depS = mktime(&currentFlight.dep);
        currentFlight.reg = content[i][2];
        currentFlight.pArriving = stoi(content[i][3]);
        currentFlight.pDeparting = stoi(content[i][4]);
        currentFlight.pTransfer = stoi(content[i][5]);
        currentFlight.type = content[i][6];
        currentFlight.body = content[i][7];
        currentFlight.flnArr = content[i][8];
        currentFlight.flnDep = content[i][9];
        flights.push_back(currentFlight);
    }
    flights.shrink_to_fit();
    return flights;
};


struct tm time_parser(string datetime){
    // assume that the input comes in the format 'DD/MM/YYYY HH:MM'
    struct tm time;
    time.tm_year = atoi(datetime.substr(6, 4).c_str())-1900;
    time.tm_mon = atoi(datetime.substr(3, 2).c_str())-1;
    time.tm_mday = atoi(datetime.substr(0,2).c_str());
    time.tm_hour = atoi(datetime.substr(11, 2).c_str());
    time.tm_min = atoi(datetime.substr(14, 2).c_str());
    if (datetime.size() > 16) {
        time.tm_sec = atoi(datetime.substr(17, 2).c_str());
    } else time.tm_sec = 0;
    // -1 value for tm_isdst means that the DST status is unknown - should not throw an error for ZRH, but may do so
    //   for airport which have 24-hour operations!!
    time.tm_isdst = -1;
    return time;
};


vector<vector<bool>> convert_to_bool(vector<vector<string>> original, const int xdim, const int ydim){
    vector<vector<bool>> converted;
    for (int i = 1; i <= xdim; ++i) {
        vector<bool> row;
        for (int j = 1; j <= ydim; ++j) {
            if (stoi(original[i][j]) == 1){
                row.push_back(true);
            } else row.push_back(false);
        }
        converted.push_back(row);
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
//    vectorMatrix.reserve(x);
    for (int i = 0; i < x; ++i) {
        vector<bool> gateAssignment;
//        gateAssignment.reserve(y);
        for (int j = 0; j < y; ++j) {
            gateAssignment.push_back(true);
        }
        gateAssignment.shrink_to_fit();
        vectorMatrix.push_back(gateAssignment);
    }
    vectorMatrix.shrink_to_fit();
    return vectorMatrix;
};


int random_number(int min, unsigned long long max){
    srand(time(NULL));   // set random seed to time, ensures every run provides different results
    int randNum = rand()%(max-min + 1) + min;
    return randNum;
};

