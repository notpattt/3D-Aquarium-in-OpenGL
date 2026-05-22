#ifndef SCENE_H
#define SCENE_H
#include <vector>
#include <glm/glm.hpp>
#include "Shader.h"

struct PebbleDef {
    glm::vec3 pos;
    glm::vec3 col;
    float rx, ry, rz;
    float rotY;
};

struct SwEntry {
    int       off;
    int       vc;
    glm::vec3 col;
    float     phase;
};

struct Bubble {
    glm::vec3 pos;
    float sz;
    float wob;
};

struct BranchDef {
    glm::vec3 pos;
    glm::vec3 col;
    float tH, tR;
    int   nb;
    float bLen, bR;
    float rotY;
};

struct CoralVAO {
    unsigned int vao;
    int          vc;
};

struct MushDef {
    glm::vec3 pos;
    glm::vec3 capCol;
    glm::vec3 spotCol;
    float sH, cR, cH, rotY;
};

struct MushVAO {
    unsigned int vao;
    int          vc;
    glm::vec3    capCol;
    glm::vec3    spotCol;
};

struct Scene {
    std::vector<PebbleDef> pebbles;

    std::vector<SwEntry>   swEntries;
    unsigned int           swVAO = 0;

    std::vector<Bubble>    bubbles;

    std::vector<BranchDef> branchDefs;
    std::vector<CoralVAO>  branchVAOs;

    std::vector<MushDef>   mushDefs;
    std::vector<MushVAO>   mushVAOs;
};

Scene buildScene();

void drawFloor(Shader& sh, unsigned int cubeVAO, unsigned int sandTex);

void drawPebbles(Shader& sh, unsigned int pebVAO, int pebVC, const std::vector<PebbleDef>& pebbles);

void drawCorals(Shader& sh, const Scene& scene);

void drawSeaweed(Shader& sh, const Scene& scene, float now);

void drawBubbles(Shader& sh, unsigned int pebVAO, int pebVC,
                 std::vector<Bubble>& bubbles, float now, float dt);

#endif