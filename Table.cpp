#include "Table.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>


void drawTable(Shader& sh, unsigned int cubeVAO, unsigned int woodTex)
{
    sh.setBool("useScales",  false);
    sh.setBool("useSpots",   false);
    sh.setBool("isSand",     false);
    sh.setBool("isGlass",    false);
    sh.setBool("useTexture", false);

    glBindVertexArray(cubeVAO);

    auto part = [&](glm::vec3 pos, glm::vec3 sc) {
        glm::mat4 m = glm::scale(glm::translate(glm::mat4(1.f), pos), sc);
        sh.setMat4("model", m);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    };

    // table
    const float BLAT_Y = -1.83f;
    const float BLAT_H = 0.20f;

    sh.setBool("useTexture", true);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, woodTex);
    part({0.f, BLAT_Y, 0.f}, {13.6f, BLAT_H, 9.6f});

    // table legs
    sh.setBool("useTexture", false);
    sh.setVec3("objectColor", 0.30f, 0.18f, 0.07f);

    const float LEG_H  = 3.0f;
    const float LEG_W  = 0.32f;
    const float LEG_Y  = BLAT_Y - BLAT_H * 0.5f - LEG_H * 0.5f;
    const float LX     = 5.9f;
    const float LZ     = 4.1f;

    part({-LX, LEG_Y, -LZ}, {LEG_W, LEG_H, LEG_W});
    part({ LX, LEG_Y, -LZ}, {LEG_W, LEG_H, LEG_W});
    part({-LX, LEG_Y,  LZ}, {LEG_W, LEG_H, LEG_W});
    part({ LX, LEG_Y,  LZ}, {LEG_W, LEG_H, LEG_W});

    // crossbars
    sh.setVec3("objectColor", 0.28f, 0.16f, 0.06f);

    const float CRS_W  = 0.16f;
    const float CRS_Y  = LEG_Y - LEG_H * 0.28f; // 28% od dołu nogi

    part({0.f, CRS_Y, -LZ}, {LX*2.f - LEG_W, CRS_W, CRS_W});
    part({0.f, CRS_Y,  LZ}, {LX*2.f - LEG_W, CRS_W, CRS_W});
    part({-LX, CRS_Y, 0.f}, {CRS_W, CRS_W, LZ*2.f - LEG_W});
    part({ LX, CRS_Y, 0.f}, {CRS_W, CRS_W, LZ*2.f - LEG_W});

    // under-counter trim
    sh.setVec3("objectColor", 0.26f, 0.15f, 0.05f);

    const float RIM_Y  = BLAT_Y - BLAT_H * 0.5f - 0.06f;
    const float RIM_H  = 0.12f;
    const float RIM_T  = 0.10f;

    part({0.f,  RIM_Y, -LZ + 0.1f}, {LX*2.f, RIM_H, RIM_T});
    part({0.f,  RIM_Y,  LZ - 0.1f}, {LX*2.f, RIM_H, RIM_T});
    part({-LX + 0.1f, RIM_Y, 0.f}, {RIM_T, RIM_H, LZ*2.f});
    part({ LX - 0.1f, RIM_Y, 0.f}, {RIM_T, RIM_H, LZ*2.f});

    // Reset
    sh.setBool("useTexture", false);
}