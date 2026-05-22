#ifndef COCONUT_H
#define COCONUT_H

#include "Shader.h"

void buildCoconutVAO(unsigned int& coconutVAO, int& coconutVC);

void drawCoconut(
    Shader&      sh,
    unsigned int coconutVAO, int coconutVC,
    unsigned int fishVAO,    int fishVC,
    unsigned int pebVAO,     int pebVC,
    unsigned int coconutTex,
    float        now
);

#endif