#include "Chest.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <cmath>
#include <vector>

using namespace Chest;

// Helpers

static unsigned int makeVAO(const std::vector<float>& v) {
    unsigned int vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, v.size()*sizeof(float), v.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
    return vao;
}

static void pushVert(std::vector<float>& v,
                     glm::vec3 p, glm::vec3 n, glm::vec2 uv) {
    v.push_back(p.x); v.push_back(p.y); v.push_back(p.z);
    v.push_back(n.x); v.push_back(n.y); v.push_back(n.z);
    v.push_back(uv.x); v.push_back(uv.y);
}

void buildTreasureVAOs(unsigned int& treasureVAO, int& treasureVC) {
    const int NX = 24, NZ = 16;
    const float IW = CW - 2*WT - 0.02f;
    const float ID = CD - 2*WT - 0.02f;

    auto heapY = [&](float tx, float tz) -> float {
        float nx = tx / (IW * 0.5f);
        float nz = tz / (ID * 0.5f);
        float d2 = nx*nx + nz*nz;
        if (d2 >= 1.0f) return 0.f;

        float bump = 0.55f * cosf(d2 * 0.85f) * (1.f - d2 * 0.5f);
        return glm::max(0.f, bump);
    };

    std::vector<float> verts;

    for (int iz = 0; iz < NZ; iz++) {
        for (int ix = 0; ix < NX; ix++) {
            float u0 = (float)ix     / NX;
            float u1 = (float)(ix+1) / NX;
            float v0 = (float)iz     / NZ;
            float v1 = (float)(iz+1) / NZ;

            float x0 = -IW*0.5f + u0*IW;
            float x1 = -IW*0.5f + u1*IW;
            float z0 = -ID*0.5f + v0*ID;
            float z1 = -ID*0.5f + v1*ID;

            float y00 = heapY(x0, z0);
            float y10 = heapY(x1, z0);
            float y01 = heapY(x0, z1);
            float y11 = heapY(x1, z1);

            glm::vec3 P00 = {x0, y00, z0};
            glm::vec3 P10 = {x1, y10, z0};
            glm::vec3 P01 = {x0, y01, z1};
            glm::vec3 P11 = {x1, y11, z1};

            auto calcN = [](glm::vec3 a, glm::vec3 b, glm::vec3 c) {
                return glm::normalize(glm::cross(b-a, c-a));
            };

            glm::vec3 n1 = calcN(P00, P10, P11);
            pushVert(verts, P00, n1, {u0,v0});
            pushVert(verts, P10, n1, {u1,v0});
            pushVert(verts, P11, n1, {u1,v1});

            glm::vec3 n2 = calcN(P00, P11, P01);
            pushVert(verts, P00, n2, {u0,v0});
            pushVert(verts, P11, n2, {u1,v1});
            pushVert(verts, P01, n2, {u0,v1});

            pushVert(verts, P11, -n1, {u1,v1});
            pushVert(verts, P10, -n1, {u1,v0});
            pushVert(verts, P00, -n1, {u0,v0});

            pushVert(verts, P01, -n2, {u0,v1});
            pushVert(verts, P11, -n2, {u1,v1});
            pushVert(verts, P00, -n2, {u0,v0});
        }
    }

    treasureVC  = (int)verts.size() / 8;
    treasureVAO = makeVAO(verts);
}


static void useWood(Shader& sh, unsigned int tex) {
    sh.setBool("useTexture", true);
    sh.setBool("useScales",  false);
    sh.setBool("useSpots",   false);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
}

static void useMetal(Shader& sh, float r, float g, float b) {
    sh.setBool("useTexture", false);
    sh.setBool("useScales",  false);
    sh.setVec3("objectColor", r, g, b);
}

static void drawCube(Shader& sh, unsigned int vao,
                     const glm::mat4& base, glm::vec3 t, glm::vec3 s) {
    glm::mat4 m = glm::scale(glm::translate(base, t), s);
    sh.setMat4("model", m);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void drawChest(Shader& sh,
               unsigned int cubeVAO,
               unsigned int treasureVAO, int treasureVC,
               unsigned int chestTex,
               unsigned int treasureTex,
               float now)
{
    glm::mat4 base = glm::mat4(1.f);
    base = glm::translate(base, glm::vec3(PX, PY + CH*0.5f, PZ));
    base = glm::rotate(base, glm::radians(YAW), glm::vec3(0.f,1.f,0.f));

    const float hx = CW*0.5f, hz = CD*0.5f;

    useWood(sh, chestTex);
    drawCube(sh,cubeVAO,base, {0,-CH*0.5f+WT*0.5f, 0}, {CW, WT, CD});  // dno
    drawCube(sh,cubeVAO,base, {0, 0, -hz+WT*0.5f},      {CW, CH, WT});  // tył
    drawCube(sh,cubeVAO,base, {0, 0,  hz-WT*0.5f},      {CW, CH, WT});  // przód
    drawCube(sh,cubeVAO,base, {-hx+WT*0.5f, 0, 0},      {WT, CH, CD});  // lewa
    drawCube(sh,cubeVAO,base, { hx-WT*0.5f, 0, 0},      {WT, CH, CD});  // prawa

    // metal parts

    useMetal(sh, 0.78f, 0.62f, 0.10f);
    float cs[4][2] = {{-hx,-hz},{hx,-hz},{-hx,hz},{hx,hz}};
    for (auto& c : cs)
        drawCube(sh,cubeVAO,base, {c[0],0,c[1]}, {0.07f,CH+0.04f,0.07f});

    useMetal(sh, 0.72f, 0.57f, 0.08f);
    for (float hy : {-CH*0.22f, CH*0.22f}) {
        for (int s : {-1,1})
            drawCube(sh,cubeVAO,base, {0,hy,s*hz}, {CW+0.02f,0.032f,0.032f});
        for (int s : {-1,1})
            drawCube(sh,cubeVAO,base, {s*hx,hy,0}, {0.032f,0.032f,CD+0.02f});
    }

    useMetal(sh, 0.88f, 0.70f, 0.12f);
    drawCube(sh,cubeVAO,base, {0,-0.05f, hz+0.01f}, {0.20f,0.20f,0.045f});
    useMetal(sh, 0.95f, 0.82f, 0.20f);
    drawCube(sh,cubeVAO,base, {0, 0.12f, hz+0.025f}, {0.11f,0.028f,0.028f});
    for (int s : {-1,1})
        drawCube(sh,cubeVAO,base, {s*0.04f,0.06f,hz+0.025f}, {0.028f,0.08f,0.028f});


    // aquarium lid

    glm::mat4 lid = glm::rotate(
        glm::translate(base, {0.f, CH*0.5f, -hz}),
        glm::radians(-105.f), {1.f,0.f,0.f});

    const float LT = 0.10f;
    useWood(sh, chestTex);
    drawCube(sh,cubeVAO,lid, {0,LT*0.5f,0}, {CW,LT,CD});

    useMetal(sh, 0.78f, 0.62f, 0.10f);
    for (auto& c : cs)
        drawCube(sh,cubeVAO,lid, {c[0],LT*0.5f,c[1]}, {0.07f,LT+0.04f,0.07f});

    useMetal(sh, 0.65f, 0.52f, 0.08f);
    for (int s : {-1,1})
        drawCube(sh,cubeVAO,base, {s*CW*0.28f,CH*0.5f,-hz}, {0.08f,0.08f,0.05f});

    // treasure

    sh.setBool("useTexture", true);
    sh.setBool("useScales",  false);
    sh.setBool("useSpots",   false);
    sh.setVec3("ambientColor", 0.55f, 0.50f, 0.45f);
    sh.setVec3("lightColor",   1.00f, 0.95f, 0.85f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, treasureTex);

    const float floorY = -CH*0.5f + WT;
    glm::mat4 tm = glm::translate(base, {0.f, floorY, 0.f});
    sh.setMat4("model", tm);

    glBindVertexArray(treasureVAO);
    glDrawArrays(GL_TRIANGLES, 0, treasureVC);

    sh.setVec3("ambientColor", 0.08f, 0.18f, 0.35f);
    sh.setVec3("lightColor",   1.00f, 0.97f, 0.90f);
    sh.setBool("useTexture", false);
    sh.setBool("useScales",  false);
}