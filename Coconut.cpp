#include "Coconut.h"
#include "Fish.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <cmath>
#include <vector>


static const glm::vec3 COCONUT_POS = { -4.2f, -1.35f, -2.0f };
static const float     COCONUT_R   =  1.80f;
static const float     COCONUT_YAW =  25.f;   // lekko w prawo (było -25)
static const float     COCONUT_TILT = 15.f;


static void pushV(std::vector<float>& v,
                  glm::vec3 p, glm::vec3 n, glm::vec2 uv) {
    v.push_back(p.x); v.push_back(p.y); v.push_back(p.z);
    v.push_back(n.x); v.push_back(n.y); v.push_back(n.z);
    v.push_back(uv.x); v.push_back(uv.y);
}


void buildCoconutVAO(unsigned int& coconutVAO, int& coconutVC) {
    std::vector<float> v;

    const int SL = 40;
    const int ST = 20;

    for (int st = 0; st < ST; st++) {
        float t0 = glm::half_pi<float>() + (float)st     / ST * glm::half_pi<float>();
        float t1 = glm::half_pi<float>() + (float)(st+1) / ST * glm::half_pi<float>();
        for (int sl = 0; sl < SL; sl++) {
            float p0 = (float)sl     / SL * glm::two_pi<float>();
            float p1 = (float)(sl+1) / SL * glm::two_pi<float>();

            auto S = [](float t, float p) {
                return glm::vec3(sinf(t)*cosf(p), cosf(t), sinf(t)*sinf(p));
            };
            glm::vec3 A=S(t0,p0), B=S(t0,p1), C=S(t1,p1), D=S(t1,p0);
            glm::vec2 uA={(float)sl/SL,     (float)st/ST};
            glm::vec2 uB={(float)(sl+1)/SL, (float)st/ST};
            glm::vec2 uC={(float)(sl+1)/SL, (float)(st+1)/ST};
            glm::vec2 uD={(float)sl/SL,     (float)(st+1)/ST};

            float R = COCONUT_R;

            pushV(v, A*R,  A, uA); pushV(v, B*R,  B, uB); pushV(v, C*R,  C, uC);
            pushV(v, A*R,  A, uA); pushV(v, C*R,  C, uC); pushV(v, D*R,  D, uD);
            float Ri = R * 0.88f;
            pushV(v, B*Ri, -B, uB); pushV(v, A*Ri, -A, uA); pushV(v, D*Ri, -D, uD);
            pushV(v, C*Ri, -C, uC); pushV(v, B*Ri, -B, uB); pushV(v, D*Ri, -D, uD);
        }
    }


    const float RO = COCONUT_R;
    const float RI = COCONUT_R * 0.85f;
    const float RD = 0.06f;

    for (int sl = 0; sl < SL; sl++) {
        float p0 = (float)sl     / SL * glm::two_pi<float>();
        float p1 = (float)(sl+1) / SL * glm::two_pi<float>();
        float xo0=cosf(p0)*RO, zo0=sinf(p0)*RO;
        float xo1=cosf(p1)*RO, zo1=sinf(p1)*RO;
        float xi0=cosf(p0)*RI, zi0=sinf(p0)*RI;
        float xi1=cosf(p1)*RI, zi1=sinf(p1)*RI;

        glm::vec3 nUp={0,1,0};
        pushV(v,{xi0,0,zi0},nUp,{0,0}); pushV(v,{xo0,0,zo0},nUp,{1,0});
        pushV(v,{xo1,0,zo1},nUp,{1,1}); pushV(v,{xi0,0,zi0},nUp,{0,0});
        pushV(v,{xo1,0,zo1},nUp,{1,1}); pushV(v,{xi1,0,zi1},nUp,{0,1});

        glm::vec3 ns0={cosf(p0),0,sinf(p0)}, ns1={cosf(p1),0,sinf(p1)};
        pushV(v,{xo0,-RD,zo0},ns0,{0,0}); pushV(v,{xo1,-RD,zo1},ns1,{1,0});
        pushV(v,{xo1, 0, zo1},ns1,{1,1}); pushV(v,{xo0,-RD,zo0},ns0,{0,0});
        pushV(v,{xo1, 0, zo1},ns1,{1,1}); pushV(v,{xo0, 0, zo0},ns0,{0,1});
    }

    coconutVC = (int)v.size() / 8;
    unsigned int vbo;
    glGenVertexArrays(1, &coconutVAO);
    glGenBuffers(1, &vbo);
    glBindVertexArray(coconutVAO);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, v.size()*sizeof(float), v.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
}


void drawCoconut(Shader& sh,
                 unsigned int coconutVAO, int coconutVC,
                 unsigned int fishVAO,    int fishVC,
                 unsigned int pebVAO,     int pebVC,
                 unsigned int coconutTex,
                 float now)
{
    sh.setBool("useSpots",   false);


    glm::mat4 base = glm::mat4(1.f);
    base = glm::translate(base, COCONUT_POS + glm::vec3(0.f, COCONUT_R * 0.72f, 0.f));
    base = glm::rotate(base, glm::radians(COCONUT_YAW),  {0.f, 1.f, 0.f});
    base = glm::rotate(base, glm::radians(90.f),          {1.f, 0.f, 0.f});
    base = glm::rotate(base, glm::radians(COCONUT_TILT),  {0.f, 1.f, 0.f});

    // textured outer shell
    sh.setBool("useTexture", true);
    sh.setBool("useScales",  false);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, coconutTex);
    sh.setMat4("model", base);
    glBindVertexArray(coconutVAO);
    glDrawArrays(GL_TRIANGLES, 0, coconutVC);

    // inner dark layer
    sh.setBool("useTexture", false);
    sh.setVec3("objectColor", 0.08f, 0.05f, 0.02f);
    glm::mat4 inner = glm::scale(base, glm::vec3(0.88f));
    sh.setMat4("model", inner);
    glDrawArrays(GL_TRIANGLES, 0, coconutVC);

    sh.setBool("useTexture", false);

    // fish inside a coconut
    {
        glm::vec4 openingDir4i = base * glm::vec4(0.f, 0.f, 1.f, 0.f);
        glm::vec3 openingDirI  = glm::normalize(glm::vec3(openingDir4i));
        glm::vec3 coconutCenterI = COCONUT_POS + glm::vec3(0.f, COCONUT_R * 0.72f, 0.f);


        glm::vec3 insideFishPos = COCONUT_POS;
        insideFishPos.y = COCONUT_POS.y + COCONUT_R * 0.55f;
        insideFishPos.y = glm::max(insideFishPos.y, -0.8f);

        glm::vec3 toObserver = -openingDirI;
        float fishYawI  = atan2f(toObserver.x, toObserver.z) + glm::radians(90.f);
        float finCycleI = sinf(now * 3.0f + 1.5f);
        float finAngleI = finCycleI * finCycleI * finCycleI * 0.3f;

        drawFish(sh,
                 fishVAO, fishVC,
                 pebVAO,  pebVC,
                 insideFishPos,
                 fishYawI,
                 0.f,
                 finAngleI * 0.2f,
                 2.8f,
                 glm::vec3(1.0f, 0.72f, 0.0f),
                 finAngleI);
    }
}