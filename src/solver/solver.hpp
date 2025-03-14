#define GLM_ENABLE_EXPERIMENTAL
#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <vector>
#include <thread>
#include <unordered_map>
#include <GLFW/glfw3.h>
#include <GL/GL.h>
#include <glm/glm.hpp>

#include "../particle/particle.hpp"
#include "../boundaries/boundaries.hpp"
#include "../threadPool/threadPool.hpp"

struct pair_hash {
    template <class T1, class T2>
    std::size_t operator() (const std::pair<T1, T2>& p) const {
        auto hash1 = std::hash<T1>{}(p.first);
        auto hash2 = std::hash<T2>{}(p.second);
        return hash1 ^ (hash2 << 1); // or use boost::hash_combine
    }
};

class Solver {
    public:
        Solver();
        ~Solver();

        Particle& addObject(glm::vec2 position, float radius);

        void render();

        void startUpdateThread();

        void addBoundary(std::unique_ptr<BoundingArea> boundary);
        std::unique_ptr<BoundingArea>& getBoundary();

        std::vector<Particle>& getObjects();
        float getStepdt();
        int getSubsteps();

        void setObjectVelocity(Particle& obj, glm::vec2 v);
        void setGravity(glm::vec2 g);
        void setStepDt(float dt);
        void setSubsteps(int substeps_);

        void mousePull(glm::vec2 position);

        private:
        std::vector<Particle> objects;
        float max_r = 0.0f;

        glm::vec2 gravity = glm::vec2({0.0f, -9.81f});
        float bounce_coefficient = 0.9f;
        float step_dt = 1.0f / 60.0f;
        
        int substeps = 8;

        ThreadPool thread_pool;
        bool update_thread_running;
        std::thread update_thread;

        std::unique_ptr<BoundingArea> bounding_area;

        float cell_size;
        std::unordered_map<std::pair<int, int>, std::vector<Particle*>, pair_hash> cell_map;

        void updateLoop();
        void update();

        void applyGravity(size_t start, size_t end);
        void applyBoundary(size_t start, size_t end);
        void updateObjects(float dt, size_t start, size_t end);

        void execInParallel(std::function<void(size_t, size_t)> func);

        void updateGrid();
        void updateCellSize(float r);
        std::pair<int, int> getCell(const glm::vec2& pos) const;
        void checkNeighbouringCells(const std::pair<int, int>& cell);

        void checkOneParticleCollision(Particle& obj, Particle& other);
        void checkAllParticleCollisions(size_t start, size_t end);

        void renderObjects();
        void renderBoundary();
};

#endif