#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
using namespace std;

// DON'T FORGET TO ADJUST NUMBER OF GATES / FLIGHTS WHEN CHANING TESTING SCENARIOS
const int ngates = 3;
const int nflights = 5;

void setInitTemp();

struct gate{
    string name;
    string conflicts[3];
};

void gateData(gate * gatesp, string * gatepathp);


int main() {
    // absolute paths to gate data and flightplan data
    string gatepath = "C:/Users/julia/OneDrive - ZHAW/MSc Engineering/VT2_AGAP/Data/Test_Gates.csv";
    string * gatepathp = &gatepath;
    string filepath = "C:/Users/julia/OneDrive - ZHAW/MSc Engineering/VT2_AGAP/Data/Flightplans/Test_Dataset_1.csv";
    string * filepathp = &filepath;

    // load gate data into array of structs
    gate gates[ngates];

    gate * gatesp = gates;
    gateData(gatesp, gatepathp);

    return 0;
}

//void setInitTemp (){
//    // initialise a SA run with temp values in the range [0.88, 0.99]
//};


void gateData(gate * gatesp, string * gatepathp){
    // create array "content", vector "row" and the "line" and "word" variables
    vector<vector<string>> content;
    vector<string> row;
    string line, word;

    // open file at gatepath and fill content
    fstream file (*gatepathp, ios::in);
    if(file.is_open()){
        while(getline(file, line)){
            row.clear();
            stringstream str(line);

            while(getline(str, word, ',')){
                row.push_back(word);
            }
            content.push_back(row);
        }
    }
    else
        cout << "Could not open file at location" << *gatepathp << endl;
    for (int i = 1; i < content.size(); ++i) {
        gatesp->name = content[i][0];
        gatesp->conflicts[0] = content[i][1];
        ++gatesp;
    }
};

