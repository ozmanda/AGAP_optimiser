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
    Solution neighbourSolution(gatesp, flightsp);
    bool solutiongenerated = false;
    int counter = 0;
    while (not solutiongenerated) {
        // choose one of three methods
        int randNum = random_number(1, 3);
        if (randNum == 1) {
            try {
                insert_move();
                solutiongenerated = true;
            } catch (logic_error) {
                if (++counter == 10){
                    cout << "Solution generation failed" << endl;
                    throw ios_base::failure("No solution generated");
                }
                continue;
            }
        } else if (randNum == 2) {
            try {
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
    candidateSolution = neighbourSolution;
    candidateSolution.set_objective_functions();
    ++generatedSolutions;
}


void Epoch::insert_move() {
    int flightIndex = random_number(0, flightsp->size());
    vector<int> availableGates;
    for (int gateIndex = 0; gateIndex < gatesp->size(); ++gateIndex) {
        if (gate_availability(flightIndex, gateIndex)) {
            availableGates.push_back(gateIndex);
        }
    }
    if (!availableGates.empty()) {
        Solution insertSolution;
        int newGateIndex = random_number(0, availableGates.size());
        for (int gateIndex = 0; gateIndex < gatesp->size(); ++gateIndex) {
            if (gateIndex == newGateIndex) {
                continue;
            } else if (currentSolution.assignment[gateIndex][flightIndex] == 1) {
                candidateSolution.assignment[gateIndex][flightIndex] = 0;
                candidateSolution.assignment[newGateIndex][flightIndex] = 1;
                break;
            }
        }
    }
}


void Epoch::interval_exchange_move() {
    interval intervalA;
    intervalA.flightIndices.push_back(random_number(0, flightsp->size()));
    set_current_gate(&intervalA);
    intervalA.t2 = (*flightsp)[intervalA.flightIndices[0]].arrS;
    intervalA.t3 = (*flightsp)[intervalA.flightIndices[0]].depS;

    interval intervalB;
    set_current_gate(&intervalB);
    intervalB.flightIndices.push_back(sample_conflicting_flight(intervalA.flightIndices[0]));
    intervalB.t2 = (*flightsp)[intervalB.flightIndices[0]].arrS;
    intervalB.t3 = (*flightsp)[intervalB.flightIndices[0]].depS;

    bool compatible = interval_compatible(&intervalA, &intervalB);
    while (not compatible) {
        int counter = 0;
        bool success = false;
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


void Epoch::next_flight(interval *flightInterval) {
    bool success = false;
    if (flightInterval->nextFlightIndex == -1) {
        // determine next flight at the gate after the interval and adjust interval values
        long long timediff = 100000;
        for (int flightIndex = 0; flightIndex < flightsp->size(); ++flightIndex) {
            if (currentSolution.assignment[flightInterval->currentGateIndex][flightIndex] == 1 and
                (*flightsp)[flightIndex].arrS > flightInterval->t3 and
                (*flightsp)[flightIndex].arrS - flightInterval->t3 < timediff) {
                flightInterval->nextFlightIndex = flightIndex;
                timediff = (*flightsp)[flightIndex].arrS - flightInterval->t3;
                success = true;
            }
        }
        if (success){
            flightInterval->t4 = (*flightsp)[flightInterval->nextFlightIndex].arrS;
        } else {
            throw logic_error("No initial next flight could be found");
        }
    } else {
        // add the index of the next flight to the interval and adjust t3
        flightInterval->flightIndices.push_back(flightInterval->nextFlightIndex);
        flightInterval->t3 = (*flightsp)[flightInterval->nextFlightIndex].arrS;

        // determine the index of the next flight and adjust interval values
        long long timediff = 100000;
        for (int flightIndex = 0; flightIndex < flightsp->size(); ++flightIndex) {
            if (currentSolution.assignment[flightInterval->currentGateIndex][flightIndex] == 1 and
                (*flightsp)[flightIndex].arrS > (*flightsp)[flightInterval->nextFlightIndex].arrS and
                (*flightsp)[flightIndex].arrS - (*flightsp)[flightInterval->nextFlightIndex].arrS < timediff) {
                flightInterval->nextFlightIndex = flightIndex;
                timediff = (*flightsp)[flightIndex].arrS - flightInterval->t3;
                success = true;
            }
        }
        if (success){
            flightInterval->t4 = (*flightsp)[flightInterval->nextFlightIndex].arrS;
        } else {
            throw logic_error("No next flight could be found");
        }
    }
}


void Epoch::prior_flight(interval *flightInterval) {
    bool success = false;
    if (flightInterval->nextFlightIndex == -1) {
        // determine previous flight at the gate before the interval and adjust interval values
        long long timediff = 100000;
        for (int flightIndex = 0; flightIndex < flightsp->size(); ++flightIndex) {
            if (currentSolution.assignment[flightInterval->currentGateIndex][flightIndex] == 1 and
                (*flightsp)[flightIndex].depS < flightInterval->t2 and
                flightInterval->t2 - (*flightsp)[flightIndex].depS < timediff) {
                flightInterval->previousFlightIndex = flightIndex;
                timediff = flightInterval->t2 - (*flightsp)[flightIndex].depS;
                success = true;
            }
        }
        if (success){
            flightInterval->t1 = (*flightsp)[flightInterval->previousFlightIndex].depS;
        } else {
            throw logic_error("No initial prior flight could be found");
        }
    } else {
        // add the index of the next flight to the interval and adjust t3
        flightInterval->flightIndices.insert(flightInterval->flightIndices.begin(),
                                             flightInterval->previousFlightIndex);
        flightInterval->t1 = (*flightsp)[flightInterval->previousFlightIndex].depS;

        // determine the index of the next flight and adjust interval values
        long long timediff = 100000;
        for (int flightIndex = 0; flightIndex < flightsp->size(); ++flightIndex) {
            if (currentSolution.assignment[flightInterval->currentGateIndex][flightIndex] == 1 and
                (*flightsp)[flightIndex].depS > (*flightsp)[flightInterval->previousFlightIndex].arrS and
                (*flightsp)[flightIndex].depS - (*flightsp)[flightInterval->previousFlightIndex].arrS < timediff) {
                flightInterval->previousFlightIndex = flightIndex;
                timediff = flightInterval->t2 - (*flightsp)[flightIndex].depS;
                success = true;
            }
        }
        if (success){
            flightInterval->t1 = (*flightsp)[flightInterval->previousFlightIndex].depS;
        } else {
            throw logic_error("No prior flight could be found");
        }
    }
}

void Epoch::apron_exchange_move() {
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
        int flightIndex = random_number(0, apronFlights.size());
        int conflictingFlightIndex = sample_conflicting_flight(flightIndex);
        // determine gate conflicting flight is assigned to
        for (int gateIndex = 0; gateIndex < gatesp->size(); ++gateIndex) {
            if (currentSolution.assignment[gateIndex][conflictingFlightIndex] == 1) {
                // switch assignment at the contact stand
                currentSolution.assignment[gateIndex][conflictingFlightIndex] = 0;
                currentSolution.assignment[gateIndex][flightIndex] = 1;
                // switch assignment on the apron
                currentSolution.assignment[0][conflictingFlightIndex] = 1;
                currentSolution.assignment[0][flightIndex] = 0;
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
    int conflictingFlightIndex = random_number(0, conflictingFlights.size());
    return conflictingFlightIndex;
}


bool Epoch::gate_availability(int flightIndex, int gateIndex) {
    // available until proven not to be
    bool available = true;

    // either the gate can handle both NB and WB or it has one size which must == that of the A/C
    if ((*gatesp)[gateIndex].body.size() != 2 and (*gatesp)[gateIndex].body[0] != (*flightsp)[flightIndex].body) {
        return false;
    }
    // for each conflicting flight ...
    for (int conflictFlight = 0; conflictFlight < (*flightsp).size(); ++conflictFlight) {
        if (flightIndex == conflictFlight) { continue; }
        if ((*flightConflictsp)[flightIndex][conflictFlight]) {
            // evaluate if it assigned to this gate
            if (currentSolution.assignment[gateIndex][flightIndex] == 1) {
                return false;
                break;
            }
            // evaluate if it assigned to a conflicting gate
            for (int conflictGate = 0; conflictGate < (*gatesp).size(); ++conflictGate) {
                if ((*gateConflictsp)[gateIndex][conflictGate] and
                    currentSolution.assignment[conflictGate][conflictFlight] == 1) {
                    return false;
                    break;
                }
            }
        }
    }
    return true;
}



