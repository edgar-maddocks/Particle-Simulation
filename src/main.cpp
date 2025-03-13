#define GLM_ENABLE_EXPERIMENTAL

#include <iostream>
#include <random>
#include <vector>
#include <memory>
#include <GLFW/glfw3.h>
#include <GL/GL.h>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "consts.cpp"
#include "boundaries/boundaries.hpp"
#include "particle/particle.hpp"
#include "solver/solver.hpp"



GLFWwindow* StartGLFW();

void setUpGL(){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

int main() {
    GLFWwindow* window = StartGLFW();
    setUpGL();

    Solver solver;

    float last_time = glfwGetTime();
    float last_spawn_time = last_time;

    solver.addBoundary(CircleBoundingArea::create(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 400.0f));

    while (!glfwWindowShouldClose(window)) {
        float current_time = glfwGetTime();
        float delta_time = current_time - last_time;
        last_time = current_time;

        if (solver.getObjects().size() < MAX_OBJECTS && current_time - last_spawn_time >= SPAWN_DELAY){
            last_spawn_time = current_time;

            auto& object = solver.addObject(SPAWN_POSITION, 15.0f);
            solver.setObjectVelocity(object, glm::vec2({1.0f, -1.0f}) * SPAWN_VELOCITY);
        }

        glClear(GL_COLOR_BUFFER_BIT);

        int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        if (state == GLFW_PRESS)
        {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            solver.mousePull(glm::vec2({xpos, SCREEN_HEIGHT - ypos}));
        }
        solver.update();
        solver.render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

GLFWwindow* StartGLFW() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return nullptr;
    }
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL Particles", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    return window;
}
