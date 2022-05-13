//
// Created by julia on 05/04/2022.
//

#ifndef OPTIMISER_ARCHIVE_H
#define OPTIMISER_ARCHIVE_H

#include "Solution.h"

class Archive {
public:
    vector<Solution> nondominated;
    vector<Solution> samples;
    void update_archive();
private:
    void attainment_surface_sampling();
};


#endif //OPTIMISER_ARCHIVE_H
