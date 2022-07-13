//
// Created by julia on 05/04/2022.
//

#include "Epoch.h"
#include "Archive.h"
#include "../src/utils.h"
#include "iostream"
#include "exception"
#include "Solution.h"
#include "cmath"
using namespace std;

// CONSTRUCTOR
Epoch::Epoch(float *initialTemperaturep, Solution *initialSolutionp, vector<Flight> *flights,
             vector<gate> *gates, vector<vector<bool>> *gateConflicts, vector<vector<bool>> *flightConflicts) {
    // assign pointers
    flightsp = flights;
    gatesp = gates;
    gateConflictsp = gateConflicts;
    flightConflictsp = flightConflicts;

    // assign temperature, current solution and initialise empty candidate solution
    temperature = *initialTemperaturep;
    currentSolution = *initialSolutionp;
    currentSolution.gatesp = gatesp;
    currentSolution.flightsp = flightsp;
    candidateSolution = Solution(gatesp, flightsp);

    // initialise archive

}


Epoch::Epoch() {

}


// public functions called by SA to conduct an epoch
Solution Epoch::run_epoch() {
    // conduct one epoch of the SA algorithm and provide current best solution to SA optimiser
    int DF = flightsp->size() * gatesp->size();
    while (true) {
        // generate new solution
        generate_neighbour();
        // evaluate acceptance
        evaluate_acceptance();
        // update archive
        archive.update_archive();
        if (acceptedSolutions == DF*12 or generatedSolutions == DF*100){
            break;
        }
    }
    // apply geometric cooling schedule
    return currentSolution;
}


void Epoch::evaluate_acceptance() {
    // create group FTilde and add members
    vector<Solution> FTilde;
    FTilde.insert(FTilde.end(),
                  make_move_iterator(archive.nondominated.begin()),
                  make_move_iterator(archive.nondominated.end()));
    FTilde.insert(FTilde.end(),
                  make_move_iterator(archive.samples.begin()),
                  make_move_iterator(archive.samples.end()));
    FTilde.push_back(currentSolution);
    FTilde.push_back(candidateSolution);
    FTilde.shrink_to_fit();

    // number of dominated solutions for candidate and current solutions
    float deltaE = calculate_deltaE(&FTilde, &candidateSolution, &currentSolution);

    if (deltaE < 0){
        cout << "Superior solution accepted" << endl;
        ++ acceptedSolutions;
        currentSolution = candidateSolution;
        candidateSolution = Solution();
        archive.nondominated = newNonDominated;
    }
    else {
        double A = exp(-deltaE / temperature);
        float a = rand() / RAND_MAX;
        if (a <= A){
            cout << "Inferior solution accepted with probability " << A << endl;
            ++acceptedSolutions;
            currentSolution = candidateSolution;
            candidateSolution = Solution();
            archive.nondominated = newNonDominated;
        } else {
            cout << "Inferior solution rejected" << endl;
        }
    }
}


float Epoch::calculate_deltaE(vector<Solution> * FTildep, Solution * candidatep, Solution * currentp){
    int FCurrent, FCandidate = 0;
    newNonDominated.clear();

    // for values in archive.nondominated
    for (int solutionIndex = 0; solutionIndex < archive.nondominated.size(); ++solutionIndex) {

        // current solution
        if (archive.nondominated[solutionIndex].apronAssignments <= currentp->apronAssignments and
            archive.nondominated[solutionIndex].gateVariance <= currentp->gateVariance){
            // check if either of the objective functions is lower and if yes, add to the counter
            if (archive.nondominated[solutionIndex].apronAssignments < currentp->apronAssignments or
                archive.nondominated[solutionIndex].gateVariance < currentp->gateVariance){
                ++ FCurrent;
            }
        }

        // candidate solution
        if (archive.nondominated[solutionIndex].apronAssignments <= candidatep->apronAssignments and
            archive.nondominated[solutionIndex].gateVariance <= candidatep->gateVariance){
            // check if either of the objective functions is lower and if yes, add to the counter
            if (archive.nondominated[solutionIndex].apronAssignments < candidatep->apronAssignments or
                archive.nondominated[solutionIndex].gateVariance < candidatep->gateVariance){
                ++ FCandidate;
            } else {
                // in this case, both objective functions MUST be equal or better
                newNonDominated.push_back((*FTildep)[solutionIndex]);
            }
        }
    }

    // for values in archive.samples
    for (int solutionIndex = 0; solutionIndex < archive.samples.size(); ++solutionIndex) {

        // current solution
        if (archive.samples[solutionIndex].apronAssignments <= currentp->apronAssignments and
            archive.samples[solutionIndex].gateVariance <= currentp->gateVariance){
            // check if either of the objective functions is lower and if yes, add to the counter
            if (archive.samples[solutionIndex].apronAssignments < currentp->apronAssignments or
                archive.samples[solutionIndex].gateVariance < currentp->gateVariance){
                ++ FCurrent;
            }
        }

        // candidate solution
        if (archive.samples[solutionIndex].apronAssignments <= candidatep->apronAssignments and
            archive.samples[solutionIndex].gateVariance <= candidatep->gateVariance){
            // check if either of the objective functions is lower and if yes, add to the counter
            if (archive.samples[solutionIndex].apronAssignments < candidatep->apronAssignments or
                archive.samples[solutionIndex].gateVariance < candidatep->gateVariance){
                ++ FCandidate;
            } else {
                // in this case, both objective functions MUST be equal or better
                newNonDominated.push_back((*FTildep)[solutionIndex]);
            }
        }
    }

    // for current and candidate solutions
    if (currentSolution.apronAssignments <= candidateSolution.apronAssignments and
        currentSolution.gateVariance <= candidateSolution.gateVariance){
        // check if either of the objective functions is lower and if yes, add to the counter
        if (currentSolution.apronAssignments < candidateSolution.apronAssignments or
            currentSolution.gateVariance < candidateSolution.gateVariance){
            ++ FCurrent;
        } else {
            // in this case, both objective functions MUST be equal or better
            newNonDominated.push_back(currentSolution);
        }
    } else {
        // in this case, the current solution MUST be superior to the candidate solution
        if (currentSolution.apronAssignments < candidateSolution.apronAssignments or
            currentSolution.gateVariance < candidateSolution.gateVariance){
            ++ FCandidate;
        }
    }


    float deltaE = (1 / FTildep->size()) * (FCurrent - FCandidate);
    return deltaE;
};


void Epoch::generate_neighbour() {
    candidateSolution = Solution(gatesp, flightsp);
    candidateSolution.assignment = currentSolution.assignment;
    bool solutiongenerated = false;
    int counter = 0;
    while (not solutiongenerated) {
        // choose one of three methods
        int randNum = random_number(1, 3);
        if (randNum == 1) {
            try {
                cout << "attempting insert move" << endl;
                insert_move();
                solutiongenerated = true;
            } catch (logic_error) {
                if (++counter == 10){
                    cout << "Solution generation failed" << endl;
                    throw ("No solution generated");
                }
                continue;
            }
        } else if (randNum == 2) {
            try {
                cout << "attempting interval exchange move" << endl;
                interval_exchange_move();
                solutiongenerated = true;
            } catch (logic_error) {
                if (++counter == 10){
                    cout << "Solution generation failed" << endl;
                    throw ios_base::failure("No solution generated");
                }
                continue;
            }
        } else if (randNum == 3) {
            try {
                cout << "attempting apron exchange move" << endl;
                apron_exchange_move();
                solutiongenerated = true;
            } catch (logic_error) {
                if (++counter == 10){
                    cout << "Solution generation failed" << endl;
                    throw ios_base::failure("No solution generated");
                }
                cout << "Apron exchange move failed, retrying" << endl;
            }
        }
    }
//    candidateSolution = neighbourSolution;
    candidateSolution.set_objective_functions();
    ++generatedSolutions;
}


void Epoch::insert_move() {
    int flightIndex = random_number(0, flightsp->size()-1);
    vector<int> availableGates;
    int currentGateIndex;
    for (int gateIndex = 1; gateIndex < gatesp->size(); ++gateIndex) {
        if (currentSolution.assignment[gateIndex][flightIndex] != 1  && gate_availability(flightIndex, gateIndex)) {
            availableGates.push_back(gateIndex);
        } else if (currentSolution.assignment[gateIndex][flightIndex] == 1){
            currentGateIndex = gateIndex;
        }
    }
    if (!availableGates.empty()) {
        // randomly select new gate from available gates, remove previous assignment and assign to new gate
        int newGateIndex = availableGates[random_number(0, availableGates.size()-1)];
        candidateSolution.assignment[currentGateIndex][flightIndex] = 0;
        candidateSolution.assignment[newGateIndex][flightIndex] = 1;
    } else throw logic_error("No available gate for insert move");
}


void Epoch::interval_exchange_move() {
    interval intervalA;
    intervalA.flightIndices.push_back(random_number(0, flightsp->size()));
    set_current_gate(&intervalA);
    if (intervalA.currentGateIndex > 5){
        set_current_gate(&intervalA);
    }
    // initialise times t2 and t3
    intervalA.t2 = (*flightsp)[intervalA.flightIndices[0]].arrS;
    intervalA.t3 = (*flightsp)[intervalA.flightIndices[0]].depS;
    // determine times t1 and t4
    prior_flight(& intervalA);
    next_flight(& intervalA);


    interval intervalB;
    int counter = 0;
    bool conflict = false;
    cout << "  Finding conflict flight" << endl;

    try {
        intervalB.flightIndices.push_back(sample_conflicting_flight(intervalA.flightIndices[0]));
        conflict = true;
    }
    catch (logic_error){
        throw logic_error("Interval exchange move failed: no conflict flight found");
    }

    set_current_gate(&intervalB);
    if (intervalB.currentGateIndex > 5){
        int x = 5;
    }
    // initialise times t2 and t3
    intervalB.t2 = (*flightsp)[intervalB.flightIndices[0]].arrS;
    intervalB.t3 = (*flightsp)[intervalB.flightIndices[0]].depS;
    // determine times t1 and t4
    prior_flight(& intervalB);
    next_flight(& intervalB);


    bool compatible = interval_compatible(&intervalA, &intervalB);
    counter = 0;
    while (not compatible) {
        if (intervalA.t2 < intervalB.t1) {
            try {
                prior_flight(&intervalB);
            } catch (logic_error){
                cout << "No prior flight could be found, interval exchange move failed" << endl;
                throw logic_error("Interval exchange move failed");
            }

        } else if (intervalB.t2 < intervalA.t1) {
            try {
                prior_flight(&intervalA);
            } catch (logic_error){
                cout << "No prior flight could be found, interval exchange move failed" << endl;
                throw logic_error("Interval exchange move failed");
            }

        } else if (intervalA.t3 > intervalB.t4) {
            try {
                next_flight(&intervalB);
            } catch (logic_error){
                cout << "No prior flight could be found, interval exchange move failed" << endl;
                throw logic_error("Interval exchange move failed");
            }

        } else if (intervalB.t3 > intervalA.t4) {
            try {
                next_flight(&intervalA);
            } catch (logic_error){
                cout << "No prior flight could be found, interval exchange move failed" << endl;
                throw logic_error("Interval exchange move failed");
            }
        }

        compatible = interval_compatible(&intervalA, &intervalB);
        if (++counter == 15 and not compatible) {
            cout << "Interval Exchange Move exceeded 20 adaptations" << endl;
        }
    }

    // once intervals are compatible, switch their gate assignments
    for (int flightIndexA = 0; flightIndexA < intervalA.flightIndices.size(); ++flightIndexA) {
        candidateSolution.assignment[intervalA.currentGateIndex][flightIndexA] = 0;
        candidateSolution.assignment[intervalB.currentGateIndex][flightIndexA] = 1;
    }
    for (int flightIndexB = 0; flightIndexB < intervalA.flightIndices.size(); ++flightIndexB) {
        candidateSolution.assignment[intervalB.currentGateIndex][flightIndexB] = 0;
        candidateSolution.assignment[intervalA.currentGateIndex][flightIndexB] = 1;
    }
}


bool Epoch::interval_compatible(interval *A, interval *B) {
    if (A->t2 < B->t1 or B->t2 < A->t1 or A->t3 > B->t4 or B->t3 > A->t4) {
        return false;
    } else return true;
}


void Epoch::set_current_gate(interval * flightInterval){
    for (int gateIndex = 0; gateIndex < gatesp->size(); ++gateIndex) {
        if (currentSolution.assignment[gateIndex][flightInterval->flightIndices[0]] == 1) {
            flightInterval->currentGateIndex = gateIndex;
            break;
        }
    }
}


void Epoch::next_flight(interval * flightInterval) {
    // if this is not the first time the function is called, extend the interval forwards
    if (flightInterval->nextFlightIndex != -1) {
        // add next flight to list of flight in interval
        flightInterval->flightIndices.push_back(flightInterval->nextFlightIndex);
        flightInterval->flightIndices.shrink_to_fit();
        // change value of t3 to new last flight
        flightInterval->t3 = (*flightsp)[flightInterval->nextFlightIndex].depS;
    }

    // determine next flight at the gate after the interval and adjust interval values
    long long timediff = 100000;
    int nextFlightIndex = -1;
    for (int flightIndex = 0; flightIndex < flightsp->size(); ++flightIndex) {
        if (flightIndex != flightInterval->flightIndices[flightInterval->flightIndices.size()] and
        currentSolution.assignment[flightInterval->currentGateIndex][flightIndex] == 1 and
        0 < (*flightsp)[flightIndex].arrS - flightInterval->t3 and
        (*flightsp)[flightIndex].arrS - flightInterval->t3 < timediff) {

            nextFlightIndex = flightIndex;
            timediff = (*flightsp)[flightIndex].arrS - flightInterval->t3;
        }
    }
    if (nextFlightIndex != -1) {
        flightInterval->t4 = (*flightsp)[nextFlightIndex].arrS;
        flightInterval->nextFlightIndex = nextFlightIndex;
    } else {
        flightInterval->t4 = 1000000000000;
    }
}


void Epoch::prior_flight(interval *flightInterval) {
    // if this is not the first time the function is called, extend the interval backwards
    if (flightInterval->previousFlightIndex != -1){
        // add prior flight to list of flights in interval
        flightInterval->flightIndices.insert(flightInterval->flightIndices.begin(),
                                             flightInterval->previousFlightIndex);
        // change value of t2 to new leading flight
        flightInterval->t2 = (*flightsp)[flightInterval->previousFlightIndex].arrS;
        bool init = true;
    }

    long long timediff = 100000;
    int priorFlightIndex = -1;
    for (int flightIndex = 0; flightIndex < flightsp->size(); ++flightIndex) {
        long long deltaT = flightInterval->t2 - (*flightsp)[flightIndex].depS;
        if (flightIndex != flightInterval->flightIndices[0] and
        currentSolution.assignment[flightInterval->currentGateIndex][flightIndex] == 1 and
        0 < flightInterval->t2 - (*flightsp)[flightIndex].depS and
        flightInterval->t2 - (*flightsp)[flightIndex].depS < timediff){
            priorFlightIndex = flightIndex;
            timediff = flightInterval->t2 - (*flightsp)[flightIndex].depS;
        }
    }
    if (priorFlightIndex != -1){
        flightInterval->t1 = (*flightsp)[priorFlightIndex].depS;
        flightInterval->previousFlightIndex = priorFlightIndex;
    } else {
        flightInterval->t1 = 0;
    }
}

void Epoch::apron_exchange_move() {
    int apronFlight = 1;
    candidateSolution.assignment = currentSolution.assignment;
    candidateSolution.assignment[2][apronFlight] = 0;
    currentSolution.assignment[2][apronFlight] = 0;
    candidateSolution.assignment[0][apronFlight] = 1;
    currentSolution.assignment[0][apronFlight] = 1;
    // find all aircraft assigned to the apron
    vector<int> apronFlights;
    for (int flightIndex = 0; flightIndex < flightsp->size(); ++flightIndex) {
        if (currentSolution.assignment[0][flightIndex] == 1){
            apronFlights.push_back(flightIndex);
        }
    }
    if (apronFlights.empty()){
        throw logic_error("No apron flights available for exchange");
    } else  {
        int flightIndex = apronFlights[random_number(0, apronFlights.size()-1)];
        int conflictingFlightIndex = sample_conflicting_flight(flightIndex);
        // determine gate conflicting flight is assigned to
        for (int gateIndex = 0; gateIndex < gatesp->size(); ++gateIndex) {
            if (currentSolution.assignment[gateIndex][conflictingFlightIndex] == 1) {
                // switch assignment at the contact stand
                candidateSolution.assignment[gateIndex][conflictingFlightIndex] = 0;
                candidateSolution.assignment[gateIndex][flightIndex] = 1;
                // switch assignment on the apron
                candidateSolution.assignment[0][conflictingFlightIndex] = 1;
                candidateSolution.assignment[0][flightIndex] = 0;
                break;
            }
        }

    }
}


int Epoch::sample_conflicting_flight(int flightIndex) {
    vector<int> conflictingFlights;
    // Find all conflicting flights
    for (int conflictingFlightIndex = 0; conflictingFlightIndex < flightsp->size(); ++conflictingFlightIndex) {
        if ((*flightConflictsp)[flightIndex][conflictingFlightIndex] == 1) {
            conflictingFlights.push_back(conflictingFlightIndex);
        }
    }
    if (conflictingFlights.size() != 0){
        int conflictingFlightIndex = conflictingFlights[random_number(0, conflictingFlights.size()-1)];
        return conflictingFlightIndex;
    } else {
        throw logic_error("No conflicting flight found");
    }
}


bool Epoch::gate_availability(int flightIndex, int gateIndex) {
    // either the gate can handle both NB and WB or it has one size which must == that of the A/C
    if ((*gatesp)[gateIndex].body.size() != 2 and (*gatesp)[gateIndex].body[0] != (*flightsp)[flightIndex].body) {
        return false;
    }
    // for each conflicting flight ...
    for (int conflictFlight = 0; conflictFlight < (*flightsp).size(); ++conflictFlight) {
        // if flight in conflict (and not identical)
        if (flightIndex != conflictFlight and (*flightConflictsp)[flightIndex][conflictFlight] == 1) {
            // evaluate if it assigned to this gate
            if (currentSolution.assignment[gateIndex][conflictFlight] == 1) {
                return false;
            }
            // evaluate if it assigned to a conflicting gate
            for (int conflictGate = 0; conflictGate < (*gatesp).size(); ++conflictGate) {
                if ((*gateConflictsp)[gateIndex][conflictGate] == 1 and
                    currentSolution.assignment[conflictGate][conflictFlight] == 1) {
                    return false;
                }
            }
        }
    }
    return true;
}



