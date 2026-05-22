#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <vector>
#include <glm/glm.hpp>

std::vector<float> buildFishVertices();
std::vector<float> buildSeaweedBlade(glm::vec3 root, float height, float width, int segs);
std::vector<float> buildBranchCoral(float trunkH, float trunkR, int numBranches, float branchLen, float branchR);
std::vector<float> buildMushroomCoral(float stalkH, float stalkR, float capR, float capH);
std::vector<float> buildPebble(float rx, float ry, float rz);

#endif