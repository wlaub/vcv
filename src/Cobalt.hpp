#pragma once

#include "TechTechTechnologies.hpp"
#include <numeric>

#define MAX_LENGTH 7

struct CobaltMessage {

    int length = 0;
    double phases[MAX_LENGTH] = {0};
    double scale = 1;
    double offset = 0;
    double outer_scale = 1;

};
