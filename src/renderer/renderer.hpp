#define GLM_ENABLE_EXPERIMENTAL
#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <GL/glew.h>

#include "../solver/solver.hpp"

class Renderer {
    public:
        Renderer(Solver& solver);
        ~Renderer();
        
        void initialize();
        void render();
    
    private:
        Solver& solver;
        GLuint vao, vbo;
        std::vector<glm::vec2> particle_positions;
    };

#endif