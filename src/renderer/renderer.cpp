#include "renderer.hpp"
#include <iostream>

Renderer::Renderer(Solver& solver) : solver(solver), vao(0), vbo(0) {
    initialize();
}

Renderer::~Renderer() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}

void Renderer::initialize() {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Renderer::render() {
    if (solver.getBoundary() != nullptr) {
        solver.renderBoundary();
    }

    particle_positions.clear();
    for (const auto& particle : solver.getObjects()) {
        particle_positions.push_back(particle.position);
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, particle_positions.size() * sizeof(glm::vec2), particle_positions.data(), GL_DYNAMIC_DRAW);

    glBindVertexArray(vao);
    glColor3f(0.0f, 1.0f, 1.0f);
    glPointSize(solver.radius + 1.0f);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glDrawArrays(GL_POINTS, 0, particle_positions.size());
    glBindVertexArray(0);
}
