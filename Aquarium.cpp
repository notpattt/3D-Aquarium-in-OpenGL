#include "Aquarium.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cmath>

void Framebuffer::init(int w, int h) {
    width = w; height = h;

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::resize(int w, int h) {
    if (w == width && h == height) return;
    width = w; height = h;

    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
}

void drawAquariumWalls(Shader& sh, unsigned int cubeVAO,
                       unsigned int reflTex, int scrW, int scrH)
{
    sh.setBool("useScales",  false);
    sh.setBool("useSpots",   false);
    sh.setBool("useTexture", false);
    sh.setBool("isSand",     false);
    sh.setBool("isGlass",    true);
    sh.setVec3("ambientColor", 0.02f, 0.12f, 0.20f);
    sh.setVec3("lightColor",   0.15f, 0.35f, 0.55f);
    sh.setVec3("objectColor",  0.75f, 0.95f, 1.00f);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, reflTex);
    sh.setInt("reflectionTex", 1);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    glBindVertexArray(cubeVAO);

    auto draw = [&](glm::vec3 pos, glm::vec3 scale) {
        glm::mat4 m = glm::translate(glm::mat4(1.f), pos);
        m = glm::scale(m, scale);
        sh.setMat4("model", m);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    };

    draw({-6.f, 1.f, 0.f},  {0.1f, 5.f, 8.f});
    draw({ 6.f, 1.f, 0.f},  {0.1f, 5.f, 8.f});
    draw({ 0.f, 1.f,-4.f},  {12.f, 5.f, 0.1f});
    draw({ 0.f, 3.5f, 0.f}, {12.f, 0.05f, 8.f});

    glDepthMask(GL_TRUE);
    glActiveTexture(GL_TEXTURE0);
    sh.setBool("isGlass", false);

    // black aquarium lid

    sh.setBool("useTexture", false);
    sh.setBool("useScales",  false);
    sh.setBool("isSand",     false);
    sh.setBool("isGlass",    false);
    sh.setVec3("ambientColor", 0.14f, 0.14f, 0.14f);
    sh.setVec3("lightColor",   0.10f, 0.10f, 0.10f);
    sh.setVec3("objectColor",  0.10f, 0.10f, 0.10f);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    draw({0.f,   3.56f,  0.f},    {12.2f, 0.10f, 8.2f});

    draw({0.f,   3.72f, -3.2f},   {12.2f, 0.42f, 1.8f});

    draw({0.f,   3.62f,  3.8f},   {12.2f, 0.22f, 0.5f});

    draw({-5.85f, 3.62f, 0.f},    {0.30f, 0.22f, 8.2f});

    draw({ 5.85f, 3.62f, 0.f},    {0.30f, 0.22f, 8.2f});

    draw({0.f,   3.68f, -1.5f},   {4.0f,  0.20f, 2.5f});


    sh.setVec3("ambientColor", 0.08f, 0.18f, 0.35f);
    sh.setVec3("lightColor",   1.00f, 0.97f, 0.90f);

    // LED strip

    sh.setBool("useTexture", false);
    sh.setBool("useScales",  false);
    sh.setBool("useSpots",   false);
    sh.setBool("isSand",     false);
    sh.setBool("isGlass",    false);

    const float LY      = 3.46f;
    const float DOT_R   = 0.035f;
    const float HALO_R  = 0.22f;
    const float SPACING = 0.32f;

    sh.setVec3("ambientColor", 0.85f, 0.82f, 0.72f);
    sh.setVec3("lightColor",   0.00f, 0.00f, 0.00f);
    sh.setVec3("objectColor",  0.90f, 0.88f, 0.78f);
    glBindVertexArray(cubeVAO);

    glm::mat4 ms = glm::scale(glm::translate(glm::mat4(1.f), {-5.96f, LY, 0.f}), {0.04f, 0.07f, 8.0f});
    sh.setMat4("model", ms); glDrawArrays(GL_TRIANGLES, 0, 36);

    ms = glm::scale(glm::translate(glm::mat4(1.f), { 5.96f, LY, 0.f}), {0.04f, 0.07f, 8.0f});
    sh.setMat4("model", ms); glDrawArrays(GL_TRIANGLES, 0, 36);

    ms = glm::scale(glm::translate(glm::mat4(1.f), {0.f, LY, -3.96f}), {12.0f, 0.07f, 0.04f});
    sh.setMat4("model", ms); glDrawArrays(GL_TRIANGLES, 0, 36);

    struct LedStrip { float xFrom, xTo, zFrom, zTo; bool alongX; float wallX, wallZ; };
    std::vector<LedStrip> strips = {
        { -5.97f, -5.97f, -3.8f,  3.8f, false, -5.97f, 0.f },  // lewa ściana
        {  5.97f,  5.97f, -3.8f,  3.8f, false,  5.97f, 0.f },  // prawa ściana
        { -5.8f,   5.8f, -3.97f,-3.97f, true,   0.f, -3.97f },  // tylna ściana
    };

    for (auto& s : strips) {
        float len   = s.alongX ? (s.xTo - s.xFrom) : (s.zTo - s.zFrom);
        int   count = (int)(fabsf(len) / SPACING) + 1;

        for (int i = 0; i < count; i++) {
            float t  = (float)i / (count > 1 ? count-1 : 1);
            float px = s.alongX ? (s.xFrom + t*(s.xTo-s.xFrom)) : s.wallX;
            float pz = s.alongX ? s.wallZ : (s.zFrom + t*(s.zTo-s.zFrom));

            sh.setVec3("ambientColor", 1.00f, 0.98f, 0.90f);
            sh.setVec3("lightColor",   0.00f, 0.00f, 0.00f);
            sh.setVec3("objectColor",  1.00f, 1.00f, 0.96f);
            glm::mat4 m = glm::scale(
                glm::translate(glm::mat4(1.f), {px, LY, pz}),
                glm::vec3(DOT_R));
            sh.setMat4("model", m);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            glDepthMask(GL_FALSE);
            sh.setVec3("ambientColor", 0.45f, 0.40f, 0.28f);
            sh.setVec3("objectColor",  0.85f, 0.78f, 0.55f);
            m = glm::scale(
                glm::translate(glm::mat4(1.f), {px, LY, pz}),
                glm::vec3(HALO_R));
            sh.setMat4("model", m);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glDepthMask(GL_TRUE);
            glDisable(GL_BLEND);
        }
    }

    sh.setVec3("ambientColor", 0.08f, 0.18f, 0.35f);
    sh.setVec3("lightColor",   1.00f, 0.97f, 0.90f);
}