#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <GL/glew.h>

#include "../solver/solver.hpp"

#include "renderer.hpp"

Renderer::Renderer(Solver& solver) : solver(solver) {
    glCreateBuffers(1, &vertex_buffer);
}

Renderer::~Renderer() {
    glDeleteBuffers(1, &vertex_buffer);
}

void Renderer::render(){
    GLfloat* vertices[50];
}

Solver& solver;

GLuint buffer;
