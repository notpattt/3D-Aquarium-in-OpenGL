#include "Fish.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

static glm::vec3 g_goldPositions[2] = { glm::vec3(0), glm::vec3(0) };

std::vector<GoldFish> initGoldFish() {
    return {
        { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 3.14f,0.0f, 0.0f, 0.0f }
    };
}

std::vector<SchoolFish> initSchoolFish() {
    return {
        // Luźny szyk — rybki wyraźnie oddalona od siebie (min ~0.7 odstępu)
        {{ 0.00f,  0.00f,  0.00f}, 0.00f, {0.30f,0.85f,1.00f}},
        {{ 0.80f, -0.15f, -0.80f}, 0.52f, {0.20f,0.95f,0.85f}},
        {{-0.80f, -0.15f, -0.80f}, 1.05f, {0.40f,0.80f,1.00f}},
        {{ 1.60f,  0.10f, -1.60f}, 1.57f, {0.15f,1.00f,0.90f}},
        {{-1.60f,  0.10f, -1.60f}, 2.09f, {0.55f,0.75f,1.00f}},
        {{ 0.70f, -0.30f, -1.60f}, 2.62f, {0.60f,0.90f,0.50f}},
        {{-0.70f, -0.30f, -1.60f}, 3.14f, {0.40f,0.85f,1.00f}},
        {{ 0.00f,  0.20f, -1.60f}, 3.67f, {0.25f,1.00f,0.80f}},
        {{ 1.40f,  0.05f, -2.40f}, 4.19f, {0.70f,0.75f,0.90f}},
        {{-1.40f,  0.05f, -2.40f}, 4.71f, {0.30f,0.90f,1.00f}},
        {{ 0.00f, -0.40f, -2.40f}, 5.24f, {0.50f,0.80f,0.60f}},
    };
}

void drawFish(Shader& sh, unsigned int fishVAO, int fishVC, unsigned int pebVAO, int pebVC,
              glm::vec3 center, float angleY, float pitch, float tailWag, float scale,
              glm::vec3 bodyColor, float finAngle)
{
    glm::mat4 base(1.f);
    base = glm::translate(base, center);
    base = glm::rotate(base, angleY,              glm::vec3(0, 1, 0)); // obrót poziomy
    base = glm::rotate(base, glm::radians(-90.f), glm::vec3(0, 1, 0)); // korekta: model ma przód w +X
    base = glm::rotate(base, -pitch,              glm::vec3(0, 0, 1)); // pochylenie góra/dół
    base = glm::rotate(base, tailWag,             glm::vec3(0, 1, 0)); // machanie ogonem
    base = glm::scale(base, glm::vec3(scale));

    glm::mat4 bodyM = base;
    bodyM = glm::scale(bodyM, glm::vec3(1.05f, 0.9f, 0.65f));
    sh.setVec3("objectColor", bodyColor.x, bodyColor.y, bodyColor.z);
    sh.setBool("useScales", true);
    sh.setMat4("model", bodyM);
    glBindVertexArray(fishVAO);
    glDrawArrays(GL_TRIANGLES, 0, fishVC);
    sh.setBool("useScales", false);

    glm::mat4 finM = base;
    finM = glm::translate(finM, glm::vec3(0.0f, 0.17f, 0.0f));
    finM = glm::rotate(finM, finAngle * 0.5f, glm::vec3(1,0,0));
    finM = glm::scale(finM, glm::vec3(0.35f, 0.12f, 0.03f));
    sh.setVec3("objectColor", bodyColor.x*0.85f, bodyColor.y*0.75f, bodyColor.z*0.5f);
    sh.setMat4("model", finM);
    glBindVertexArray(pebVAO);
    glDrawArrays(GL_TRIANGLES, 0, pebVC);

    glm::mat4 finL = base;
    finL = glm::translate(finL, glm::vec3(-0.02f, -0.06f, 0.07f));
    finL = glm::rotate(finL, -finAngle*0.8f, glm::vec3(0,1,0));
    finL = glm::rotate(finL, glm::radians(-20.f), glm::vec3(1,0,0));
    finL = glm::rotate(finL, glm::radians(20.f), glm::vec3(0,0,1));
    finL = glm::scale(finL, glm::vec3(0.25f, 0.05f, 0.18f));
    sh.setVec3("objectColor", bodyColor.x*0.9f, bodyColor.y*0.8f, bodyColor.z*0.55f);
    sh.setMat4("model", finL);
    glDrawArrays(GL_TRIANGLES, 0, pebVC);

    glm::mat4 finR = base;
    finR = glm::translate(finR, glm::vec3(-0.02f, -0.06f, -0.07f));
    finR = glm::rotate(finR, finAngle*0.8f, glm::vec3(0,1,0));
    finR = glm::rotate(finR, glm::radians(-20.f), glm::vec3(1,0,0));
    finR = glm::rotate(finR, glm::radians(20.f), glm::vec3(0,0,1));
    finR = glm::scale(finR, glm::vec3(0.25f, 0.05f, 0.18f));
    sh.setMat4("model", finR);
    glDrawArrays(GL_TRIANGLES, 0, pebVC);

    auto drawEye = [&](float zOffset, float outDir) {
        glm::mat4 eyeBase = base;
        eyeBase = glm::translate(eyeBase, glm::vec3(0.28f, 0.06f, zOffset));
        eyeBase = glm::scale(eyeBase, glm::vec3(0.06f, 0.06f, 0.035f));
        sh.setVec3("objectColor", 0.75f, 0.75f, 0.8f);
        sh.setMat4("model", eyeBase);
        glDrawArrays(GL_TRIANGLES, 0, pebVC);

        glm::mat4 pupil = base;
        pupil = glm::translate(pupil, glm::vec3(0.285f, 0.06f, zOffset + (0.02f * outDir)));
        pupil = glm::scale(pupil, glm::vec3(0.035f, 0.035f, 0.02f));
        sh.setVec3("objectColor", 0.02f, 0.02f, 0.02f);
        sh.setMat4("model", pupil);
        glDrawArrays(GL_TRIANGLES, 0, pebVC);

        glm::mat4 highlight = base;
        highlight = glm::translate(highlight, glm::vec3(0.295f, 0.075f, zOffset + (0.035f * outDir)));
        highlight = glm::scale(highlight, glm::vec3(0.012f, 0.012f, 0.01f));
        sh.setVec3("objectColor", 1.0f, 1.0f, 1.0f);
        sh.setMat4("model", highlight);
        glDrawArrays(GL_TRIANGLES, 0, pebVC);
    };

    drawEye( 0.055f,  1.0f);
    drawEye(-0.055f, -1.0f);
}


static bool nearCoconut(float x, float z) {
    const float cx = -4.2f, cz = -2.0f, cr = 2.2f; // trochę większy margines
    float dx = x - cx, dz = z - cz;
    return (dx*dx + dz*dz) < cr*cr;
}

static glm::vec3 avoidCoconut(glm::vec3 pos) {
    const float cx = -4.2f, cz = -2.0f, cr = 2.2f;
    float dx = pos.x - cx, dz = pos.z - cz;
    float d2 = dx*dx + dz*dz;
    if (d2 < cr*cr && d2 > 0.001f) {
        float d = sqrtf(d2);
        float nx = dx/d, nz = dz/d;
        pos.x = cx + nx * cr;
        pos.z = cz + nz * cr;
    }
    return pos;
}

void updateAndDrawGoldFish(std::vector<GoldFish>& goldFish, float now, float baseSpeed,
                           float fishSpeedMultiplier, Shader& sh,
                           unsigned int fishVAO, int fishVC,
                           unsigned int pebVAO,  int pebVC)
{

    auto getGoldfishPos = [&](int id, float t) -> glm::vec3 {
        if (id == 0) {
            return glm::vec3(
                -1.5f + sinf(t * 0.28f) * 2.8f,          // X ∈ [-4.3, 1.3]
                 1.2f + sinf(t * 0.41f) * 0.5f,           // Y ∈ [0.7, 1.7]
                 0.5f + cosf(t * 0.22f) * 2.0f            // Z ∈ [-1.5, 2.5]
            );
        } else {
            return glm::vec3(
                 1.5f + cosf(t * 0.30f + glm::pi<float>()) * 2.8f, // X ∈ [-1.3, 4.3]
                 0.5f + cosf(t * 0.38f + glm::pi<float>()) * 0.4f, // Y ∈ [0.1, 0.9]
                 0.0f + sinf(t * 0.25f + glm::pi<float>()) * 2.2f  // Z ∈ [-2.2, 2.2]
            );
        }
    };

    static glm::vec3 prevPos[2]   = { glm::vec3(0), glm::vec3(0) };
    static float     prevAngle[2] = { 0.f, 0.f };
    static float     prevPitch[2] = { 0.f, 0.f };

    glm::vec3 rawPos[2];
    for (int i = 0; i < (int)goldFish.size(); i++) {
        float t    = now * baseSpeed * 1.2f;
        glm::vec3 p = getGoldfishPos(i, t);
        p.x = glm::clamp(p.x, -4.5f, 4.5f);
        p.y = glm::clamp(p.y,  0.2f,  2.8f);
        p.z = glm::clamp(p.z, -3.0f,  3.0f);
        rawPos[i] = avoidCoconut(p);
    }

    const float GOLD_SEP = 1.2f;
    for (int i = 0; i < (int)goldFish.size(); i++) {
        for (int j = 0; j < (int)goldFish.size(); j++) {
            if (i == j) continue;
            glm::vec3 diff = rawPos[i] - rawPos[j];
            float d = glm::length(diff);
            if (d < GOLD_SEP && d > 0.001f)
                rawPos[i] += glm::normalize(diff) * (GOLD_SEP - d) * 0.5f;
        }
    }

    for (int i = 0; i < (int)goldFish.size(); i++) {
        glm::vec3 pos = rawPos[i];

        float t = now * baseSpeed * 1.2f;
        glm::vec3 nextRaw = getGoldfishPos(i, t + 0.04f);
        nextRaw.x = glm::clamp(nextRaw.x, -4.5f, 4.5f);
        nextRaw.y = glm::clamp(nextRaw.y,  0.2f,  2.8f);
        nextRaw.z = glm::clamp(nextRaw.z, -3.0f,  3.0f);
        nextRaw   = avoidCoconut(nextRaw);

        glm::vec3 dir = nextRaw - pos;
        float len = glm::length(dir);
        if (len < 0.0001f) dir = glm::vec3(0,0,1);
        else dir /= len;

        float targetAngle = atan2f(dir.x, dir.z);
        float hSpd        = sqrtf(dir.x*dir.x + dir.z*dir.z) + 0.001f;
        float targetPitch = atan2f(dir.y, hSpd) * 0.5f;

        float da = targetAngle - prevAngle[i];
        while (da >  glm::pi<float>()) da -= glm::two_pi<float>();
        while (da < -glm::pi<float>()) da += glm::two_pi<float>();
        prevAngle[i] += da * 0.08f;
        prevPitch[i] += (targetPitch - prevPitch[i]) * 0.06f;

        float finCycle = sinf(now * 4.0f * fishSpeedMultiplier + (float)i * 2.0f);
        float finAngle = finCycle * finCycle * finCycle * 0.4f;

        drawFish(sh, fishVAO, fishVC, pebVAO, pebVC,
                 pos, prevAngle[i], prevPitch[i], finAngle*0.25f, 2.1f,
                 glm::vec3(1.0f, 0.72f, 0.0f), finAngle);

        g_goldPositions[i] = pos; // zapisz dla funkcji ławicy
    }
}

void updateAndDrawSchoolFish(std::vector<SchoolFish>& school, float now, float baseSpeed,
                             float fishSpeedMultiplier, Shader& sh,
                             unsigned int fishVAO, int fishVC,
                             unsigned int pebVAO,  int pebVC)
{
    auto getSchoolPos = [&](float t) -> glm::vec3 {
        return glm::vec3(
            1.0f + sinf(t * 0.3f) * 2.0f,   // X ∈ [-1, 3] — bezpieczny środek
            1.5f + cosf(t * 0.4f) * 0.4f,   // Y ∈ [1.1, 1.9] — zawsze wysoko nad dnem
            0.5f + sinf(t * 0.25f) * 1.0f   // Z ∈ [-0.5, 1.5] — środek akwarium
        );
    };

    float schoolT        = now * baseSpeed * 0.9f;
    glm::vec3 schoolPos  = getSchoolPos(schoolT);
    schoolPos.x = glm::clamp(schoolPos.x, -3.0f, 3.0f);
    schoolPos.y = glm::clamp(schoolPos.y,  0.5f,  2.5f);
    schoolPos.z = glm::clamp(schoolPos.z, -2.5f,  2.5f);
    schoolPos   = avoidCoconut(schoolPos);

    glm::vec3 schoolNext = getSchoolPos(schoolT + 0.04f);
    schoolNext.x = glm::clamp(schoolNext.x, -3.0f, 3.0f);
    schoolNext.y = glm::clamp(schoolNext.y,  0.5f,  2.5f);
    schoolNext.z = glm::clamp(schoolNext.z, -2.5f,  2.5f);
    schoolNext   = avoidCoconut(schoolNext);

    glm::vec3 sDir = schoolNext - schoolPos;
    float sLen = glm::length(sDir);
    if (sLen < 0.0001f) sDir = glm::vec3(0,0,1);
    else sDir /= sLen;

    static float schoolAngle = 0.f;
    static float schoolPitchS = 0.f;
    float targetSchoolAngle = atan2f(sDir.x, sDir.z);
    float da = targetSchoolAngle - schoolAngle;
    while (da >  glm::pi<float>()) da -= glm::two_pi<float>();
    while (da < -glm::pi<float>()) da += glm::two_pi<float>();
    schoolAngle  += da * 0.06f;
    float sHSpd   = sqrtf(sDir.x*sDir.x + sDir.z*sDir.z) + 0.001f;
    schoolPitchS += (atan2f(sDir.y, sHSpd) * 0.5f - schoolPitchS) * 0.05f;

    glm::mat4 formRot = glm::mat4(1.0f);
    formRot = glm::rotate(formRot, schoolAngle,   glm::vec3(0, 1, 0));
    formRot = glm::rotate(formRot, schoolPitchS,  glm::vec3(1, 0, 0));

    std::vector<glm::vec3> fishPositions;
    fishPositions.reserve(school.size());

    for (auto& sf : school) {
        glm::vec4 slotLocal(sf.offset.x, sf.offset.y, sf.offset.z, 1.0f);
        glm::vec4 slotWorld = formRot * slotLocal;
        float indT = now * baseSpeed * 1.8f + sf.phase;
        float microX = sinf(indT * 1.3f + sf.phase * 2.1f) * 0.12f
                     + sinf(indT * 2.7f + sf.phase * 0.7f) * 0.05f;
        float microY = cosf(indT * 1.8f + sf.phase * 1.4f) * 0.10f
                     + sinf(indT * 3.2f + sf.phase * 2.3f) * 0.04f;
        float microZ = sinf(indT * 1.5f + sf.phase * 1.8f) * 0.10f;
        fishPositions.push_back({
            schoolPos.x + slotWorld.x + microX,
            schoolPos.y + slotWorld.y + microY,
            schoolPos.z + slotWorld.z + microZ
        });
    }

    const float SEP  = 0.55f;
    const float GSEP = 1.0f;

    for (int fi = 0; fi < (int)school.size(); fi++) {
        glm::vec3 sep(0.f);
        for (int fj = 0; fj < (int)fishPositions.size(); fj++) {
            if (fi == fj) continue;
            glm::vec3 diff = fishPositions[fi] - fishPositions[fj];
            float d = glm::length(diff);
            if (d < SEP && d > 0.001f)
                sep += glm::normalize(diff) * (SEP - d) * 0.5f;
        }
        for (auto& gp : g_goldPositions) {
            glm::vec3 diff = fishPositions[fi] - gp;
            float d = glm::length(diff);
            if (d < GSEP && d > 0.001f)
                sep += glm::normalize(diff) * (GSEP - d) * 0.8f;
        }
        fishPositions[fi] += sep;
    }

    for (int fi = 0; fi < (int)school.size(); fi++) {
        auto& sf = school[fi];
        glm::vec3 fpos = fishPositions[fi];

        const float SEP_DIST = 0.55f;
        glm::vec3 sep(0.f);
        for (int fj = 0; fj < (int)fishPositions.size(); fj++) {
            if (fi == fj) continue;
            glm::vec3 diff = fpos - fishPositions[fj];
            float d = glm::length(diff);
            if (d < SEP_DIST && d > 0.001f) {
                sep += glm::normalize(diff) * (SEP_DIST - d) * 0.5f;
            }
        }
        fpos += sep;

        fpos = avoidCoconut(fpos);
        fpos.x = glm::clamp(fpos.x, -5.0f,  5.0f);
        fpos.y = glm::clamp(fpos.y,  0.2f,  3.0f);
        fpos.z = glm::clamp(fpos.z, -3.5f,  3.5f);

        float indT     = now * baseSpeed * 1.8f + sf.phase;
        float indYaw   = schoolAngle  + sinf(indT * 0.9f + sf.phase) * 0.10f;
        float indPitch = schoolPitchS + sinf(indT * 1.4f + sf.phase) * 0.06f;

        float fc2   = sinf(now * 5.5f * fishSpeedMultiplier + sf.phase);
        float finA2 = fc2 * fc2 * fc2 * 0.45f;

        drawFish(sh, fishVAO, fishVC, pebVAO, pebVC,
                 fpos, indYaw, indPitch, finA2*0.22f, 0.62f, sf.col, finA2);
    }
}