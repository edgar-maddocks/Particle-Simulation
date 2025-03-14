#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <tuple>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "../constants/constants.hpp"
#include "../solver/solver.hpp"

GLFWwindow* StartGLFW();
void setUpGL(const std::tuple<float, float, float, float> background_rgb);
float generateRandom(const float max, const float min);
void spawnParticles(Solver& solver, float current_time, float last_spawn_time);
void gravityMousePull(Solver& solver, GLFWwindow* window);

#endif