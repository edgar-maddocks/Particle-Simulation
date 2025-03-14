#define GLM_ENABLE_EXPERIMENTAL

#include <iostream>
#include <random>
#include <vector>
#include <tuple>
#include <memory>
#include <GLFW/glfw3.h>
#include <GL/GL.h>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "../constants/constants.hpp"
#include "utils.cpp"
#include "boundaries/boundaries.hpp"
#include "particle/particle.hpp"
#include "solver/solver.hpp"
#include "simulators/airfoil/airfoil.hpp"

GLFWwindow* StartGLFW();


int main() {
    GLFWwindow* window = StartGLFW();
    setUpGL(std::make_tuple(0.0f, 0.0f, 0.0f, 0.0f));

    AirFoilSimulator simulator = AirFoilSimulator();
    simulator.setStepDt(1.0f / 120.0f);
    simulator.setSubsteps(8);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        
        simulator.update();
        simulator.render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}