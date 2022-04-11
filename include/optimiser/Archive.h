//
// Created by julia on 05/04/2022.
//

#ifndef OPTIMISER_ARCHIVE_H
#define OPTIMISER_ARCHIVE_H

//#include "Solution.h"

class Archive() {
    Archive(float temp);
    float temperature;
    Solution * front;
    Solution * sample;

    void attainment_surface_sampling();
    void update_archive();
};


#endif //OPTIMISER_ARCHIVE_H
