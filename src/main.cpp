#define GLM_ENABLE_EXPERIMENTAL

#include <iostream>
#include <random>
#include <tuple>
#include <vector>
#include <memory>
#include <GLFW/glfw3.h>
#include <GL/GL.h>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "consts.cpp"
#include "utils.cpp"
#include "boundaries/boundaries.hpp"
#include "particle/particle.hpp"
#include "solver/solver.hpp"



GLFWwindow* StartGLFW();


int main() {
    GLFWwindow* window = StartGLFW();
    setUpGL(std::make_tuple(1.0f, 1.0f, 1.0f, 1.0f));

    Solver solver;

    float last_time = glfwGetTime();
    float last_spawn_time = last_time;

    solver.addBoundary(CircleBoundingArea::create(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 400.0f));

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        
        float current_time = glfwGetTime();
        float delta_time = current_time - last_time;
        last_time = current_time;

        if (solver.getObjects().size() < MAX_OBJECTS && current_time - last_spawn_time >= SPAWN_DELAY){
            last_spawn_time = current_time;

            auto& object = solver.addObject(SPAWN_POSITION, 15.0f);
            solver.setObjectVelocity(object, glm::vec2({1.0f, -1.0f}) * SPAWN_VELOCITY);
        }

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


