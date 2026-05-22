#ifndef FISH_H
#define FISH_H

#include <vector>
#include <glm/glm.hpp>
#include "Shader.h"

struct GoldFish {
    float orbitR, orbitSpd, phase, yBase, yAmp, yFreq;
};

struct SchoolFish {
    glm::vec3 offset;
    float phase;
    glm::vec3 col;
};

std::vector<GoldFish>   initGoldFish();
std::vector<SchoolFish> initSchoolFish();

void drawFish(Shader& sh,
              unsigned int fishVAO, int fishVC,
              unsigned int pebVAO,  int pebVC,
              glm::vec3 center,
              float angleY,
              float pitch,
              float tailWag,
              float scale,
              glm::vec3 bodyColor,
              float finAngle);

void updateAndDrawGoldFish(std::vector<GoldFish>& goldFish,
                           float now, float baseSpeed, float fishSpeedMultiplier,
                           Shader& sh,
                           unsigned int fishVAO, int fishVC,
                           unsigned int pebVAO,  int pebVC);

void updateAndDrawSchoolFish(std::vector<SchoolFish>& school,
                             float now, float baseSpeed, float fishSpeedMultiplier,
                             Shader& sh,
                             unsigned int fishVAO, int fishVC,
                             unsigned int pebVAO,  int pebVC);

#endif