#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

struct Camera {
    glm::vec3 pos   = glm::vec3(0.f, 1.f, 6.f);
    glm::vec3 front = glm::vec3(0.f, -0.15f, -1.f);
    glm::vec3 up    = glm::vec3(0.f, 1.f,  0.f);
    float fov       = 60.f;

    float yaw        = -90.f;
    float pitch      = -8.f;
    float lastX      = 512.f;
    float lastY      = 384.f;
    bool  firstMouse    = true;
    float sensitivity   = 0.08f;

    bool  mouseCaptured = true;
    bool  tabPressed    = false;

    void onMouseMove(GLFWwindow* /*w*/, double xd, double yd) {
        if (!mouseCaptured) return;
        float x = (float)xd, y = (float)yd;
        if (firstMouse) { lastX = x; lastY = y; firstMouse = false; return; }
        yaw   += (x - lastX) * sensitivity;
        pitch += (lastY - y) * sensitivity;
        lastX  = x;
        lastY  = y;
        if (pitch >  89.f) pitch =  89.f;
        if (pitch < -89.f) pitch = -89.f;
        front.x = cosf(glm::radians(yaw)) * cosf(glm::radians(pitch));
        front.y = sinf(glm::radians(pitch));
        front.z = sinf(glm::radians(yaw)) * cosf(glm::radians(pitch));
        front   = glm::normalize(front);
    }

    void onScroll(double /*dx*/, double dy) {
        fov -= (float)dy * 2.f;
        if (fov < 10.f) fov = 10.f;
        if (fov > 90.f) fov = 90.f;
    }

    // Zwraca mnożnik prędkości ryb (SPACE = 3x)
    float processInput(GLFWwindow* w, float dt) {
        if (glfwGetKey(w, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(w, true);

        if (glfwGetKey(w, GLFW_KEY_TAB) == GLFW_PRESS && !tabPressed) {
            tabPressed    = true;
            mouseCaptured = !mouseCaptured;
            firstMouse    = true;
            glfwSetInputMode(w, GLFW_CURSOR,
                mouseCaptured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        }
        if (glfwGetKey(w, GLFW_KEY_TAB) == GLFW_RELEASE) tabPressed = false;

        if (mouseCaptured) {
            float sp = 3.f * dt;
            glm::vec3 right = glm::normalize(glm::cross(front, up));
            if (glfwGetKey(w, GLFW_KEY_W) == GLFW_PRESS) pos += sp * front;
            if (glfwGetKey(w, GLFW_KEY_S) == GLFW_PRESS) pos -= sp * front;
            if (glfwGetKey(w, GLFW_KEY_A) == GLFW_PRESS) pos -= right * sp;
            if (glfwGetKey(w, GLFW_KEY_D) == GLFW_PRESS) pos += right * sp;
            if (glfwGetKey(w, GLFW_KEY_Q) == GLFW_PRESS) pos -= up * sp;
            if (glfwGetKey(w, GLFW_KEY_E) == GLFW_PRESS) pos += up * sp;
        }

        return (glfwGetKey(w, GLFW_KEY_SPACE) == GLFW_PRESS) ? 3.f : 1.f;
    }

    glm::mat4 viewMatrix() const {
        return glm::lookAt(pos, pos + front, up);
    }

    glm::mat4 projMatrix(float aspect) const {
        return glm::perspective(glm::radians(fov), aspect, 0.1f, 100.f);
    }
};

#endif