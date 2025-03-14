#define GLM_ENABLE_EXPERIMENTAL
#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "../solver/solver.hpp"

class Renderer {
    public:
        Renderer(Solver& solver);
        ~Renderer();

        void render();

    private:
        Solver& solver;

        GLuint vertex_buffer;


};

#endif