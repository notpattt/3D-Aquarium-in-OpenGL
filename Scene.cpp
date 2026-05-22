#include "Scene.h"
#include "Geometry.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <cstdlib>
#include <cmath>


static unsigned int uploadSceneVAO(const std::vector<float>& v) {
    unsigned int vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(float), v.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);                  glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));  glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float)));  glEnableVertexAttribArray(2);
    return vao;
}


Scene buildScene() {
    Scene sc;

    auto tooCloseToCoconut = [](float x, float z) {
        float dx = x-(-4.2f), dz = z-(-2.0f);
        return (dx*dx+dz*dz) < 2.4f*2.4f;
    };
    auto tooCloseToChest = [](float x, float z) {
        float dx = x-4.6f, dz = z-(-2.8f);
        return (dx*dx+dz*dz) < 2.0f*2.0f;
    };
    auto tooClose = [&](float x, float z) {
        return tooCloseToCoconut(x,z) || tooCloseToChest(x,z);
    };

    // rocks — various sizes
    const std::vector<glm::vec3> pebCols = {
        {0.42f,0.38f,0.35f},{0.50f,0.46f,0.42f},{0.38f,0.35f,0.32f},
        {0.58f,0.54f,0.50f},{0.35f,0.33f,0.31f},{0.48f,0.44f,0.40f},
        {0.62f,0.58f,0.54f},{0.30f,0.28f,0.26f},{0.55f,0.50f,0.46f},
    };

    // small gravel
    for (int i = 0; i < 80; i++) {
        float px = ((rand()%1100)/100.f) - 5.5f;
        float pz = ((rand()%700) /100.f) - 3.5f;
        float r  = .018f + (rand()%6)*.005f;
        glm::vec3 col = pebCols[rand()%pebCols.size()];
        col += glm::vec3(((rand()%16)-8)*.004f,((rand()%16)-8)*.004f,((rand()%16)-8)*.004f);
        sc.pebbles.push_back({glm::vec3(px,-1.37f,pz), col, r, r*0.55f, r, (float)(rand()%360)});
    }
    // medium-sized rocks
    for (int i = 0; i < 40; i++) {
        float px = ((rand()%1100)/100.f) - 5.5f;
        float pz = ((rand()%700) /100.f) - 3.5f;
        float rx = .06f + (rand()%8)*.012f;
        float ry = .025f + (rand()%5)*.008f;
        float rz = .055f + (rand()%7)*.010f;
        glm::vec3 col = pebCols[rand()%pebCols.size()];
        col += glm::vec3(((rand()%20)-10)*.005f,((rand()%20)-10)*.005f,((rand()%20)-10)*.005f);
        sc.pebbles.push_back({glm::vec3(px,-1.37f,pz), col, rx, ry, rz, (float)(rand()%360)});
    }
    // large rocks
    struct BoulderPos { float x, z; };
    std::vector<BoulderPos> boulders = {
        {-4.8f, 3.2f},{-4.5f,-0.8f},{-3.8f,-2.8f},
        { 4.8f, 2.8f},{ 4.5f,-0.5f},{ 3.8f,-3.0f},
        {-1.5f,-3.6f},{ 1.0f,-3.5f},{ 0.0f, 3.4f},
    };
    for (auto& b : boulders) {
        if (tooClose(b.x, b.z)) continue;
        float rx = .18f + (rand()%8)*.025f;
        float ry = .08f + (rand()%5)*.018f;
        float rz = .16f + (rand()%7)*.022f;
        glm::vec3 col = pebCols[rand()%pebCols.size()];
        col *= 0.75f; // głazy ciemniejsze
        sc.pebbles.push_back({glm::vec3(b.x,-1.36f,b.z), col, rx, ry, rz, (float)(rand()%360)});
    }

    // seaweed
    struct GreenPatch { float cx, cz; int n; float hMin, hMax; glm::vec3 c1, c2; };
    std::vector<GreenPatch> patches = {
        // left wall
        {-5.0f, 3.0f, 12, 0.8f, 2.5f, {0.04f,0.50f,0.04f},{0.12f,0.80f,0.10f}},
        {-5.0f, 1.0f, 12, 0.7f, 2.2f, {0.05f,0.55f,0.05f},{0.14f,0.84f,0.12f}},
        {-5.0f,-1.0f, 10, 0.8f, 2.4f, {0.04f,0.52f,0.04f},{0.14f,0.82f,0.12f}},
        {-4.5f, 2.0f, 10, 0.6f, 1.8f, {0.05f,0.55f,0.06f},{0.14f,0.84f,0.12f}},
        {-4.5f,-0.5f,  8, 0.7f, 2.0f, {0.05f,0.58f,0.05f},{0.15f,0.86f,0.13f}},
        // right wall
        { 5.0f, 3.0f, 12, 0.8f, 2.5f, {0.04f,0.50f,0.04f},{0.12f,0.80f,0.10f}},
        { 5.0f, 1.0f, 12, 0.7f, 2.2f, {0.05f,0.55f,0.05f},{0.14f,0.84f,0.12f}},
        { 5.0f,-1.0f, 10, 0.8f, 2.4f, {0.04f,0.52f,0.04f},{0.14f,0.82f,0.12f}},
        { 4.5f, 2.0f, 10, 0.6f, 1.8f, {0.05f,0.55f,0.06f},{0.14f,0.84f,0.12f}},
        { 4.5f,-0.5f,  8, 0.7f, 2.0f, {0.05f,0.58f,0.05f},{0.15f,0.86f,0.13f}},
        // back wall
        {-2.5f,-3.3f, 10, 1.2f, 3.0f, {0.04f,0.60f,0.08f},{0.18f,0.90f,0.14f}},
        { 0.0f,-3.5f, 10, 1.4f, 3.2f, {0.05f,0.65f,0.10f},{0.20f,0.95f,0.15f}},
        { 2.5f,-3.3f, 10, 1.2f, 3.0f, {0.04f,0.60f,0.08f},{0.18f,0.90f,0.14f}},
        // front corners
        {-3.5f, 3.2f,  8, 0.5f, 1.6f, {0.06f,0.56f,0.06f},{0.15f,0.84f,0.13f}},
        { 3.5f, 3.2f,  8, 0.5f, 1.6f, {0.06f,0.56f,0.06f},{0.15f,0.84f,0.13f}},
        // center
        { 0.5f, 0.5f,  3, 0.3f, 0.8f, {0.06f,0.58f,0.06f},{0.15f,0.85f,0.13f}},
        {-1.0f,-1.0f,  3, 0.3f, 0.8f, {0.06f,0.58f,0.06f},{0.15f,0.85f,0.13f}},
    };
    std::vector<float> swAll;
    for (auto& p : patches) {
        int placed = 0, attempts = 0;
        while (placed < p.n && attempts < p.n * 15) {
            attempts++;
            float ox = ((rand()%100)/100.f - 0.5f) * 1.5f;
            float oz = ((rand()%100)/100.f - 0.5f) * 1.5f;
            float fx = p.cx + ox, fz = p.cz + oz;
            if (fx < -5.5f || fx > 5.5f || fz < -3.8f || fz > 3.8f) continue;
            if (tooClose(fx, fz)) continue;
            float h   = p.hMin + (rand()%100)/100.f * (p.hMax - p.hMin);
            float w   = .06f + (rand()%100)/100.f * .18f;
            float ph  = (rand()%100)/100.f * glm::two_pi<float>();
            glm::vec3 col = glm::mix(p.c1, p.c2, (rand()%100)/100.f);
            auto bv   = buildSeaweedBlade({fx, -1.35f, fz}, h, w, 10);
            int  off  = (int)swAll.size() / 8;
            int  vc   = (int)bv.size() / 8;
            swAll.insert(swAll.end(), bv.begin(), bv.end());
            sc.swEntries.push_back({off, vc, col, ph});
            placed++;
        }
    }
    // Upload seaweed
    unsigned int swVBO;
    glGenVertexArrays(1, &sc.swVAO);
    glGenBuffers(1, &swVBO);
    glBindVertexArray(sc.swVAO);
    glBindBuffer(GL_ARRAY_BUFFER, swVBO);
    glBufferData(GL_ARRAY_BUFFER, swAll.size()*sizeof(float), swAll.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)0);                 glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(3*sizeof(float))); glEnableVertexAttribArray(1);
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(6*sizeof(float))); glEnableVertexAttribArray(2);

    // bubbles
    for (int i = 0; i < 40; i++) {
        sc.bubbles.push_back({
            {((rand()%100)/100.f - 0.5f) * 10.0f,   // X ∈ [-5, 5]
             -1.2f + (rand()%100)/100.f * 2.0f,       // Y ∈ [-1.2, 0.8]
             ((rand()%100)/100.f - 0.5f) * 7.0f},     // Z ∈ [-3.5, 3.5]
            .015f + (rand()%6)*.008f,
            (rand()%100)/100.f * glm::two_pi<float>()
        });
    }

    // branched corals
    sc.branchDefs = {
        // left wall
        {{-5.0f,-1.35f, 2.5f},{1.0f,0.35f,0.45f}, 1.8f,.08f,9,.70f,.045f,  10},  // różowy
        {{-5.0f,-1.35f, 0.5f},{0.95f,0.55f,0.20f},1.6f,.07f,8,.65f,.042f, -15},  // pomarańczowy
        {{-5.0f,-1.35f,-1.5f},{0.85f,0.25f,0.55f},1.5f,.07f,8,.62f,.040f,  20},  // fiolet-różowy
        {{-4.2f,-1.35f, 1.5f},{1.0f,0.45f,0.30f}, 1.4f,.07f,7,.58f,.038f,  -5},  // łososiowy
        {{-4.2f,-1.35f,-0.5f},{0.70f,0.20f,0.80f},1.3f,.065f,7,.55f,.036f,  35}, // fioletowy
        // right wall
        {{ 5.0f,-1.35f, 2.5f},{1.0f,0.30f,0.50f}, 1.8f,.08f,9,.70f,.045f, -10},
        {{ 5.0f,-1.35f, 0.5f},{0.90f,0.50f,0.15f},1.6f,.07f,8,.65f,.042f,  15},
        {{ 5.0f,-1.35f,-1.5f},{0.80f,0.20f,0.60f},1.5f,.07f,8,.62f,.040f, -20},
        {{ 4.2f,-1.35f, 1.5f},{1.0f,0.40f,0.35f}, 1.4f,.07f,7,.58f,.038f,   5},
        {{ 4.2f,-1.35f,-0.5f},{0.65f,0.25f,0.85f},1.3f,.065f,7,.55f,.036f, -35},
        // back wall
        {{-2.0f,-1.35f,-3.5f},{1.0f,0.20f,0.40f}, 1.6f,.075f,8,.65f,.042f,   0},
        {{ 0.5f,-1.35f,-3.5f},{0.55f,0.10f,0.90f},1.8f,.080f,9,.72f,.046f,  25},
        {{ 2.8f,-1.35f,-3.5f},{1.0f,0.45f,0.20f}, 1.5f,.070f,7,.60f,.040f, -10},
        // center
        {{-1.5f,-1.35f,-1.0f},{1.0f,0.30f,0.45f}, 1.0f,.055f,6,.45f,.030f,  15},
        {{ 1.5f,-1.35f,-1.5f},{0.40f,0.10f,0.95f},1.1f,.058f,6,.48f,.032f, -20},
        {{ 2.5f,-1.35f, 1.5f},{0.95f,0.50f,0.15f},0.9f,.050f,5,.42f,.028f,  40},
    };

    // mushroom corals
    sc.mushDefs = {
        // left wall
        {{-4.8f,-1.35f, 3.0f},{0.95f,0.35f,0.55f},{1.0f,0.75f,0.85f},1.10f,.80f,.42f,  15},
        {{-4.5f,-1.35f, 1.0f},{1.0f, 0.60f,0.20f},{1.0f,0.90f,0.60f},0.95f,.70f,.38f, -20},
        {{-4.8f,-1.35f,-1.0f},{0.75f,0.20f,0.80f},{0.90f,0.65f,0.95f},1.00f,.75f,.40f,  30},
        {{-3.5f,-1.35f, 2.0f},{0.90f,0.45f,0.30f},{1.0f,0.80f,0.70f},0.75f,.56f,.30f,  -5},
        {{-3.5f,-1.35f,-0.5f},{0.50f,0.80f,0.55f},{0.75f,1.0f,0.78f},0.65f,.48f,.26f,  50},
        // right wall
        {{ 4.8f,-1.35f, 3.0f},{0.90f,0.30f,0.60f},{1.0f,0.70f,0.85f},1.10f,.80f,.42f, -15},
        {{ 4.5f,-1.35f, 1.0f},{1.0f, 0.55f,0.15f},{1.0f,0.88f,0.58f},0.95f,.70f,.38f,  20},
        {{ 4.8f,-1.35f,-1.0f},{0.70f,0.15f,0.85f},{0.88f,0.62f,0.95f},1.00f,.75f,.40f, -30},
        {{ 3.5f,-1.35f, 2.0f},{0.85f,0.40f,0.35f},{1.0f,0.78f,0.70f},0.75f,.56f,.30f,   5},
        {{ 3.5f,-1.35f,-0.5f},{0.45f,0.78f,0.50f},{0.72f,1.0f,0.75f},0.65f,.48f,.26f, -50},
        // back wall
        {{-1.8f,-1.35f,-3.2f},{1.0f,0.25f,0.45f},{1.0f,0.72f,0.82f},0.90f,.66f,.35f,  10},
        {{ 1.5f,-1.35f,-3.0f},{0.60f,0.15f,0.90f},{0.82f,0.60f,1.0f},1.05f,.76f,.40f, -25},
        {{ 3.5f,-1.35f,-3.2f},{1.0f,0.50f,0.18f},{1.0f,0.85f,0.55f},0.80f,.60f,.32f,  35},
        // center
        {{-1.5f,-1.35f,-0.5f},{1.0f,0.30f,0.50f},{1.0f,0.75f,0.85f},0.55f,.40f,.22f,  10},
        {{ 1.5f,-1.35f,-1.8f},{0.35f,0.72f,1.0f},{0.72f,0.92f,1.0f},0.70f,.50f,.27f,  35},
        {{ 2.3f,-1.35f,-1.2f},{0.85f,0.15f,0.88f},{1.0f,0.68f,1.0f},0.55f,.40f,.22f, -15},
        {{-0.5f,-1.35f, 1.5f},{1.0f,0.60f,0.12f},{1.0f,0.90f,0.52f},0.85f,.62f,.34f,  -5},
        {{ 0.0f,-1.35f,-1.5f},{0.30f,0.85f,0.60f},{0.68f,1.0f,0.80f},0.45f,.34f,.18f,  70},
    };

    // mushrooms
    {
        std::vector<MushDef> candidates = sc.mushDefs;
        sc.mushDefs.clear();
        sc.mushVAOs.clear();
        for (auto& d : candidates) {
            if (tooClose(d.pos.x, d.pos.z)) continue;
            bool collision = false;
            for (auto& a : sc.mushDefs) {
                float dx = d.pos.x - a.pos.x, dz = d.pos.z - a.pos.z;
                float minD = d.cR + a.cR + 0.50f;
                if (dx*dx + dz*dz < minD*minD) { collision = true; break; }
            }
            if (collision) continue;
            auto mv = buildMushroomCoral(d.sH, .065f, d.cR, d.cH);
            sc.mushVAOs.push_back({uploadSceneVAO(mv), (int)mv.size()/8, d.capCol, d.spotCol});
            sc.mushDefs.push_back(d);
        }
    }

    for (auto& d : sc.branchDefs) {
        if (tooClose(d.pos.x, d.pos.z)) continue;
        auto cv = buildBranchCoral(d.tH, d.tR, d.nb, d.bLen, d.bR);
        sc.branchVAOs.push_back({uploadSceneVAO(cv), (int)cv.size()/8});
    }
    {
        std::vector<BranchDef> filtered;
        for (auto& d : sc.branchDefs)
            if (!tooClose(d.pos.x, d.pos.z))
                filtered.push_back(d);
        sc.branchDefs = filtered;
    }

    return sc;
}

void drawFloor(Shader& sh, unsigned int cubeVAO, unsigned int sandTex) {
    sh.setBool("useTexture", true);
    sh.setBool("useScales",  false);
    sh.setBool("isSand",     true);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sandTex);
    glm::mat4 m = glm::translate(glm::mat4(1.f), {0.f, -1.5f, 0.f});
    m = glm::scale(m, {12.f, .3f, 8.f});
    sh.setMat4("model", m);
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    sh.setBool("isSand",    false);   // reset
    sh.setBool("useTexture", false);
}

void drawPebbles(Shader& sh, unsigned int pebVAO, int pebVC,
                 const std::vector<PebbleDef>& pebbles)
{
    glBindVertexArray(pebVAO);
    for (auto& p : pebbles) {
        sh.setVec3("objectColor", p.col.x, p.col.y, p.col.z);
        glm::mat4 m = glm::translate(glm::mat4(1.f), p.pos);
        m = glm::rotate(m, glm::radians(p.rotY), {0.f,1.f,0.f});
        m = glm::scale(m, {p.rx, p.ry, p.rz});
        sh.setMat4("model", m);
        glDrawArrays(GL_TRIANGLES, 0, pebVC);
    }
}

void drawCorals(Shader& sh, const Scene& sc) {
    for (int i = 0; i < (int)sc.branchDefs.size(); i++) {
        glm::mat4 m = glm::translate(glm::mat4(1.f), sc.branchDefs[i].pos);
        m = glm::rotate(m, glm::radians(sc.branchDefs[i].rotY), {0.f,1.f,0.f});
        sh.setMat4("model", m);
        sh.setVec3("objectColor", sc.branchDefs[i].col.x, sc.branchDefs[i].col.y, sc.branchDefs[i].col.z);
        glBindVertexArray(sc.branchVAOs[i].vao);
        glDrawArrays(GL_TRIANGLES, 0, sc.branchVAOs[i].vc);
    }
    for (int i = 0; i < (int)sc.mushDefs.size(); i++) {
        glm::mat4 m = glm::translate(glm::mat4(1.f), sc.mushDefs[i].pos);
        m = glm::rotate(m, glm::radians(sc.mushDefs[i].rotY), {0.f,1.f,0.f});
        sh.setMat4("model", m);
        sh.setVec3("objectColor", sc.mushVAOs[i].capCol.x, sc.mushVAOs[i].capCol.y, sc.mushVAOs[i].capCol.z);
        sh.setVec3("spotColor",   sc.mushVAOs[i].spotCol.x, sc.mushVAOs[i].spotCol.y, sc.mushVAOs[i].spotCol.z);
        sh.setBool("useSpots", true);
        glBindVertexArray(sc.mushVAOs[i].vao);
        glDrawArrays(GL_TRIANGLES, 0, sc.mushVAOs[i].vc);
        sh.setBool("useSpots", false);
    }
}


void drawSeaweed(Shader& sh, const Scene& sc, float now) {
    (void)now;
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0f, 1.0f);

    glBindVertexArray(sc.swVAO);
    sh.setMat4("model", glm::mat4(1.f));
    for (auto& sw : sc.swEntries) {
        sh.setFloat("seaweedPhase", sw.phase);
        sh.setVec3("objectColor", sw.col.x, sw.col.y, sw.col.z);
        glDrawArrays(GL_TRIANGLES, sw.off, sw.vc);
    }

    glDisable(GL_POLYGON_OFFSET_FILL);
}


void drawBubbles(Shader& sh, unsigned int pebVAO, int pebVC,
                 std::vector<Bubble>& bubbles, float now, float dt)
{
    glBindVertexArray(pebVAO);
    sh.setVec3("objectColor", .65f, .88f, 1.f);
    sh.setBool("useScales", false);
    for (auto& b : bubbles) {
        float wob = sinf(now * 1.5f + b.wob) * .12f;
        b.pos.y  += 1.1f * dt;
        if (b.pos.y > 3.0f) {
            b.pos.y = -1.2f;
            b.pos.x = glm::clamp(b.pos.x, -5.0f, 5.0f);
            b.pos.z = glm::clamp(b.pos.z, -3.5f, 3.5f);
        }
        float bx = glm::clamp(b.pos.x + wob * 0.04f, -5.0f, 5.0f);
        glm::mat4 m = glm::translate(glm::mat4(1.f), {bx, b.pos.y, b.pos.z});
        m = glm::scale(m, glm::vec3(b.sz));
        sh.setMat4("model", m);
        glDrawArrays(GL_TRIANGLES, 0, pebVC);
    }
}