#define GLM_ENABLE_EXPERIMENTAL

#include <iostream>
#include <iomanip> 
#include <random>
#include <tuple>
#include <vector>
#include <memory>
#include <thread>
#include <GLFW/glfw3.h>
#include <GL/GL.h>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "constants/constants.hpp"
#include "utils/utils.hpp"
#include "boundaries/boundaries.hpp"
#include "particle/particle.hpp"
#include "solver/solver.hpp"

GLFWwindow* StartGLFW();

int main() {
    GLFWwindow* window = StartGLFW();
    setUpGL(std::make_tuple(1.0f, 1.0f, 1.0f, 1.0f));

    Solver solver;
    std::cout << std::thread::hardware_concurrency() << std::endl;

    float last_time = glfwGetTime();
    float last_spawn_time = last_time;

    solver.addBoundary(CircleBoundingArea::create(GraphicsConstants::SCREEN_WIDTH/2, GraphicsConstants::SCREEN_HEIGHT/2, 400.0f));
    solver.startUpdateThread();

    while (!glfwWindowShouldClose(window)) {
        float frame_start_time = glfwGetTime();

        glClear(GL_COLOR_BUFFER_BIT);
        
        float current_time = glfwGetTime();
        float delta_time = current_time - last_time;
        last_time = current_time;

        spawnParticles(solver, current_time, last_spawn_time);

        gravityMousePull(solver, window);

        solver.render();

        glfwSwapBuffers(window);
        glfwPollEvents();

        float frame_end_time = glfwGetTime(); // End time of the frame
        float fps = 1.0f / (frame_end_time - frame_start_time); // Time taken for the frame
        std::cout << "\rFPS: " << std::fixed << std::setprecision(3) << fps << " Number of Particles: " << solver.getObjects().size() << "          " << std::flush; // Print frame time in milliseconds
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}


