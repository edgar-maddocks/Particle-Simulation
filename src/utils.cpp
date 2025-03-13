#include <iostream>
#include <tuple>
#include <GLFW/glfw3.h>

#include "consts.cpp"

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

void setUpGL(const std::tuple<float, float, float, float> background_rgb){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, -1, 1);
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