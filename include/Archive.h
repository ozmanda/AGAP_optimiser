//
// Created by julia on 05/04/2022.
//

#ifndef OPTIMISER_ARCHIVE_H
#define OPTIMISER_ARCHIVE_H

#include "Solution.h"

class Archive {
public:
    void update_archive();
private:
    vector<Solution> nondominating;
    vector<Solution> samples;
    void attainment_surface_sampling();
    float temperature;

};


#endif //OPTIMISER_ARCHIVE_H
