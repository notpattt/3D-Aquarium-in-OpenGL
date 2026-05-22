#ifndef AQUARIUM_H
#define AQUARIUM_H
#include <glad/glad.h>
#include "Shader.h"

struct Framebuffer {
    unsigned int fbo    = 0;
    unsigned int tex    = 0;
    unsigned int rbo    = 0;
    int          width  = 0;
    int          height = 0;

    void init(int w, int h);
    void resize(int w, int h);
    void bind()   const { glBindFramebuffer(GL_FRAMEBUFFER, fbo); }
    void unbind() const { glBindFramebuffer(GL_FRAMEBUFFER, 0);   }
};

void drawAquariumWalls(Shader& sh, unsigned int cubeVAO,
                       unsigned int reflTex, int scrW, int scrH);

#endif