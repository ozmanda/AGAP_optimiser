#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include "../include/Flight.h"
#include "../include/SA.h"
#include "../src/utils.h"
using namespace std;

void setInitTemp();

vector<gate> gateData(string * gatepathp);
vector<Flight> flightData(string * flightplanpathp);
struct tm time_parser(string time);


int main() {
    // absolute paths to gate data and flightplan data
    string gatepath = "C:/Users/julia/OneDrive - ZHAW/MSc Engineering/VT2_AGAP/Data/Test_Gates.csv";
    string gateconflictpath = "C:/Users/julia/OneDrive - ZHAW/MSc Engineering/VT2_AGAP/Data/Test_Gate_Conflicts.csv";
    string flightplanpath = "C:/Users/julia/OneDrive - ZHAW/MSc Engineering/VT2_AGAP/Data/Flightplans/Test_Dataset_1.csv";

    // load gate and flight data into vectors
    vector<gate> gates = gateData(&gatepath);
    vector<Flight> flights = flightData(&flightplanpath);

    // initialise SA optimiser with gates and flights
    SA optimiser(gates, flights, &gateconflictpath);
    optimiser.set_initial_solution();

    return 0;
}

//void setInitTemp (){
//    // initialise a SA run with temp values in the range [0.88, 0.99]
//};


vector<gate> gateData(string * gatepathp){
    vector<vector<string>> content = parse_file(gatepathp);
    vector<gate> gates;
    // iterate through content and create gate struct to push back on the vector
    gate currentGate;
    for (int i = 1; i < content.size(); ++i) {
        currentGate.name = content[i][0];
        currentGate.conflicts = comma_separate(& content[i][1]);
//        currentGate.conflicts.push_back(content[i][1]);
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

