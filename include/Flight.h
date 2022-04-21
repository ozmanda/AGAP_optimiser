//
// Created by julia on 06/04/2022.
//

#ifndef OPTIMISER_FLIGHT_H
#define OPTIMISER_FLIGHT_H

#include "ctime"
#include "string"
using namespace std;

class Flight {
public:
    string reg;
    struct tm arr, dep;
    time_t arrS, depS;
    int pArriving, pDeparting, pTransfer;
    string flnArr, flnDep, body, type;
    Flight * conflictingFlight();
};


#endif //OPTIMISER_FLIGHT_H
