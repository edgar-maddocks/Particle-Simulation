#include <iostream>
#include <tuple>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "../constants/constants.hpp"
#include "../solver/solver.hpp"

#include "utils.hpp"

GLFWwindow* StartGLFW() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return nullptr;
    }
    GLFWwindow* window = glfwCreateWindow(GraphicsConstants::SCREEN_WIDTH, GraphicsConstants::SCREEN_HEIGHT, "OpenGL Particles", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    return window;
}

void setUpGL(const std::tuple<float, float, float, float> background_rgb){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, GraphicsConstants::SCREEN_WIDTH, 0, GraphicsConstants::SCREEN_HEIGHT, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClearColor(
        std::get<0>(background_rgb), 
        std::get<1>(background_rgb), 
        std::get<2>(background_rgb), 
        std::get<3>(background_rgb)
    );
}

float generateRandom(const float max, const float min){
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(max - min)));
}

void spawnParticles(Solver& solver, float current_time, float last_spawn_time){
    if (solver.getObjects().size() < SolverConstants::MAX_OBJECTS && current_time - last_spawn_time >= SolverConstants::SPAWN_DELAY){
        last_spawn_time = current_time;

        auto& object = solver.addObject(SolverConstants::SPAWN_POSITION, 3.0f);
        solver.setObjectVelocity(object, glm::vec2({1.0f, -1.0f}) * SolverConstants::SPAWN_VELOCITY);
    }
}

void gravityMousePull(Solver& solver, GLFWwindow* window){
    int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        if (state == GLFW_PRESS)
    {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        solver.mousePull(glm::vec2({xpos, GraphicsConstants::SCREEN_HEIGHT - ypos}));
    }
}