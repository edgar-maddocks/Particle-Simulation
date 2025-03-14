#define GLM_ENABLE_EXPERIMENTAL

#include <iostream>
#include <iomanip> 
#include <random>
#include <tuple>
#include <vector>
#include <memory>
#include <thread>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "constants/constants.hpp"
#include "utils/utils.hpp"
#include "boundaries/boundaries.hpp"
#include "particle/particle.hpp"
#include "solver/solver.hpp"

#include "renderer/renderer.hpp"

GLFWwindow* StartGLFW();

int main() {
    GLFWwindow* window = StartGLFW();
    setUpGL(std::make_tuple(1.0f, 1.0f, 1.0f, 1.0f));

    Solver solver(5.0f);
    Renderer renderer(solver);

    float last_time = glfwGetTime();
    float last_spawn_time = last_time;

    solver.addBoundary(CircleBoundingArea::create(GraphicsConstants::SCREEN_WIDTH/2, GraphicsConstants::SCREEN_HEIGHT/2, 700.0f));
    solver.startUpdateThread();

    while (!glfwWindowShouldClose(window)) {
        float frame_start_time = glfwGetTime();

        glClear(GL_COLOR_BUFFER_BIT);
        
        float current_time = glfwGetTime();
        float delta_time = current_time - last_time;
        last_time = current_time;

        spawnParticles(solver, current_time, last_spawn_time);

        gravityMousePull(solver, window);

        renderer.render();

        glfwSwapBuffers(window);
        glfwPollEvents();

        float frame_end_time = glfwGetTime(); // End time of the frame
        float frame_time = (frame_end_time - frame_start_time) * 1000.0f; // Time taken for the frame
        std::cout << "\rFrame Time: " << std::fixed << std::setprecision(3) << frame_time << "ms | Number of Particles: " << solver.getObjects().size() << "          " << std::flush; // Print frame time in milliseconds
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}


