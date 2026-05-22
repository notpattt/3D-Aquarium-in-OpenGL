#ifndef CHEST_H
#define CHEST_H

#include <vector>
#include <glm/glm.hpp>
#include "Shader.h"

namespace Chest {
    constexpr float PX  =  4.6f;
    constexpr float PY  = -1.355f;
    constexpr float PZ  = -2.8f;
    constexpr float YAW = -20.f;

    constexpr float CW  = 2.0f;
    constexpr float CH  = 0.75f;
    constexpr float CD  = 1.3f;
    constexpr float WT  = 0.09f;
}

void buildTreasureVAOs(unsigned int& treasureVAO, int& treasureVC);

void drawChest(
    Shader&      sh,
    unsigned int cubeVAO,
    unsigned int treasureVAO, int treasureVC,
    unsigned int chestTex,
    unsigned int treasureTex,
    float        now
);

#endif