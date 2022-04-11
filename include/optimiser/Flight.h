//
// Created by julia on 06/04/2022.
//

#ifndef OPTIMISER_FLIGHT_H
#define OPTIMISER_FLIGHT_H

#include "ctime"
#include "string"
using namespace std;

class Flight {
    string reg;
    tm arr, dep;
    int pArriving, pDeparting, pTransfer;
    string flightNumbers, body, type;
    Flight * conflictingFlight();
};


#endif //OPTIMISER_FLIGHT_H
