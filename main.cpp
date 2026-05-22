#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Shader.h"
#include "Geometry.h"
#include "TextureLoader.h"
#include "Fish.h"
#include "Camera.h"
#include "VAOBuilder.h"
#include "Scene.h"
#include "Chest.h"
#include "Aquarium.h"
#include "Coconut.h"
#include "Table.h"

#include <iostream>
#include <cstdlib>

// Globals
static unsigned int SCR_W = 1024, SCR_H = 768;
static Camera       g_cam;
static bool         g_filterActive     = false;
static bool         g_filterKeyPressed = false;


// GLFW callbacks
void mouse_callback(GLFWwindow*, double x, double y) {
    g_cam.onMouseMove(nullptr, x, y);
}
void scroll_callback(GLFWwindow*, double dx, double dy) {
    g_cam.onScroll(dx, dy);
}


static void setCommon(Shader& sh, const Camera& cam, float now,
                      unsigned int w, unsigned int h)
{
    sh.setMat4("projection", cam.projMatrix((float)w / (float)h));
    sh.setMat4("view",       cam.viewMatrix());
    sh.setVec3("viewPos",    cam.pos);
    sh.setVec3("lightPos",   sinf(now * .5f) * 6.f, 6.f, cosf(now * .5f) * 6.f);
    sh.setVec3("lightColor",   1.0f, 0.97f, 0.90f);
    sh.setVec3("ambientColor", 0.08f, 0.18f, 0.35f);
    sh.setFloat("time", now);
    sh.setInt("texture1",      0);
    sh.setInt("reflectionTex", 1);
    glUniform2f(glGetUniformLocation(sh.ID, "screenSize"), (float)w, (float)h);
    sh.setBool("useTexture", false);
    sh.setBool("useSpots",   false);
    sh.setBool("useScales",  false);
    sh.setBool("isSand",     false);
    sh.setBool("isGlass",    false);
}


// main
int main() {
    // GLFW / OpenGL init
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_W, SCR_H,
        "Akwarium 3D — Rafa Koralowa", nullptr, nullptr);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window,    scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Shaders
    Shader sceneShader  ("scene.vert",   "scene.frag");
    Shader seaweedShader("seaweed.vert", "scene.frag");
    Shader screenShader ("screen.vert",  "screen.frag");

    // basic VAO
    float cubeV[] = {
        -.5f,-.5f,-.5f,0,0,-1,0,0, .5f,-.5f,-.5f,0,0,-1,1,0, .5f,.5f,-.5f,0,0,-1,1,1,
         .5f,.5f,-.5f,0,0,-1,1,1, -.5f,.5f,-.5f,0,0,-1,0,1, -.5f,-.5f,-.5f,0,0,-1,0,0,
        -.5f,-.5f,.5f,0,0,1,0,0, .5f,-.5f,.5f,0,0,1,1,0, .5f,.5f,.5f,0,0,1,1,1,
         .5f,.5f,.5f,0,0,1,1,1, -.5f,.5f,.5f,0,0,1,0,1, -.5f,-.5f,.5f,0,0,1,0,0,
        -.5f,.5f,.5f,-1,0,0,1,0, -.5f,.5f,-.5f,-1,0,0,1,1, -.5f,-.5f,-.5f,-1,0,0,0,1,
        -.5f,-.5f,-.5f,-1,0,0,0,1, -.5f,-.5f,.5f,-1,0,0,0,0, -.5f,.5f,.5f,-1,0,0,1,0,
         .5f,.5f,.5f,1,0,0,1,0, .5f,.5f,-.5f,1,0,0,1,1, .5f,-.5f,-.5f,1,0,0,0,1,
         .5f,-.5f,-.5f,1,0,0,0,1, .5f,-.5f,.5f,1,0,0,0,0, .5f,.5f,.5f,1,0,0,1,0,
        -.5f,-.5f,-.5f,0,-1,0,0,1, .5f,-.5f,-.5f,0,-1,0,1,1, .5f,-.5f,.5f,0,-1,0,1,0,
         .5f,-.5f,.5f,0,-1,0,1,0, -.5f,-.5f,.5f,0,-1,0,0,0, -.5f,-.5f,-.5f,0,-1,0,0,1,
        -.5f,.5f,-.5f,0,1,0,0,1, .5f,.5f,-.5f,0,1,0,1,1, .5f,.5f,.5f,0,1,0,1,0,
         .5f,.5f,.5f,0,1,0,1,0, -.5f,.5f,.5f,0,1,0,0,0, -.5f,.5f,-.5f,0,1,0,0,1,
    };
    float quadV[] = { -1,1,0,1, -1,-1,0,0, 1,-1,1,0, -1,1,0,1, 1,-1,1,0, 1,1,1,1 };

    unsigned int cubeVAO = uploadVAO(cubeV, sizeof(cubeV), false);
    unsigned int quadVAO = uploadVAO(quadV, sizeof(quadV), true);

    auto fishVerts   = buildFishVertices();
    int  fishVC      = (int)fishVerts.size() / 8;
    unsigned int fishVAO = uploadVAO(fishVerts, false);

    auto pebVerts    = buildPebble(1, 1, 1);
    int  pebVC       = (int)pebVerts.size() / 8;
    unsigned int pebVAO = uploadVAO(pebVerts, false);

    // treasure geometry
    unsigned int treasureVAO;
    int          treasureVC;
    buildTreasureVAOs(treasureVAO, treasureVC);

    // coconut geometry
    unsigned int coconutVAO;
    int          coconutVC;
    buildCoconutVAO(coconutVAO, coconutVC);

    // scene data
    srand(12345);
    Scene scene    = buildScene();

    std::vector<GoldFish>   goldFish = initGoldFish();
    std::vector<SchoolFish> school   = initSchoolFish();

    // textures
    unsigned int sandTex  = loadTexture("texture2.jpg");
    if (!sandTex) sandTex = loadTexture("texture.jpg");
    unsigned int chestTex    = loadTexture("texture.jpg");
    unsigned int treasureTex = loadTexture("texture3.jpg");
    unsigned int coconutTex  = loadTexture("texture4.jpg");

    // Framebuffer
    Framebuffer fb;
    fb.init((int)SCR_W, (int)SCR_H);

    float fishSpeedMul = 1.f;
    float dt = 0.f, lastTime = 0.f;


    // main loop
    while (!glfwWindowShouldClose(window)) {
        // window resizing support
        int cW, cH;
        glfwGetFramebufferSize(window, &cW, &cH);
        if (cW > 0 && cH > 0) {
            SCR_W = (unsigned)cW;
            SCR_H = (unsigned)cH;
            glViewport(0, 0, cW, cH);
            fb.resize(cW, cH);
        }

        float now = (float)glfwGetTime();
        dt        = now - lastTime;
        lastTime  = now;

        // Input
        fishSpeedMul = g_cam.processInput(window, dt);
        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && !g_filterKeyPressed) {
            g_filterActive     = !g_filterActive;
            g_filterKeyPressed = true;
        }
        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE) g_filterKeyPressed = false;

        // Render to framebuffer
        fb.bind();
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.f, 0.07f, 0.22f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        sceneShader.use();
        setCommon(sceneShader, g_cam, now, SCR_W, SCR_H);

        drawFloor(sceneShader, cubeVAO, sandTex);
        drawPebbles(sceneShader, pebVAO, pebVC, scene.pebbles);
        drawChest(sceneShader,
                  cubeVAO, treasureVAO, treasureVC,
                  chestTex, treasureTex, now);
        drawCorals(sceneShader, scene);

        float baseSpeed = 0.28f * fishSpeedMul;
        updateAndDrawGoldFish(goldFish, now, baseSpeed, fishSpeedMul,
                              sceneShader, fishVAO, fishVC, pebVAO, pebVC);
        updateAndDrawSchoolFish(school, now, baseSpeed, fishSpeedMul,
                                sceneShader, fishVAO, fishVC, pebVAO, pebVC);

        drawBubbles(sceneShader, pebVAO, pebVC, scene.bubbles, now, dt);

        // a coconut with a fish
        drawCoconut(sceneShader,
                    coconutVAO, coconutVC,
                    fishVAO, fishVC,
                    pebVAO, pebVC,
                    coconutTex,
                    now);

        seaweedShader.use();
        setCommon(seaweedShader, g_cam, now, SCR_W, SCR_H);
        seaweedShader.setBool("isSand", false);
        drawSeaweed(seaweedShader, scene, now);

        // table
        sceneShader.use();
        setCommon(sceneShader, g_cam, now, SCR_W, SCR_H);
        drawTable(sceneShader, cubeVAO, chestTex);

        // glass walls
        sceneShader.use();
        setCommon(sceneShader, g_cam, now, SCR_W, SCR_H);
        drawAquariumWalls(sceneShader, cubeVAO, fb.tex, (int)SCR_W, (int)SCR_H);

        // Post-processing
        fb.unbind();
        glDisable(GL_DEPTH_TEST);
        glClearColor(1, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        screenShader.use();
        screenShader.setBool("applyFilter", g_filterActive);
        glBindVertexArray(quadVAO);
        glBindTexture(GL_TEXTURE_2D, fb.tex);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}