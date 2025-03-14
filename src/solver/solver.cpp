#define GLM_ENABLE_EXPERIMENTAL

#include <vector>
#include <memory>
#include <algorithm>
#include <thread>
#include <unordered_map>
#include <GLFW/glfw3.h>
#include <GL/GL.h>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "../particle/particle.hpp"
#include "../boundaries/boundaries.hpp"
#include "../threadPool/threadPool.hpp"

#include "solver.hpp"

Solver::Solver(float radius_) 
: thread_pool(std::thread::hardware_concurrency() - 1) // subtract 1 for the update thread
, update_thread_running(false)
, radius(radius_)
, cell_size(2 * radius_)
{};

Solver::~Solver(){
    update_thread_running = false;
    if (update_thread.joinable()){
        update_thread.join();
    }
}

Particle& Solver::addObject(glm::vec2 position){
    Particle new_particle = Particle(position, radius);
    return objects.emplace_back(new_particle);
}

void Solver::renderBoundary(){
    const int bounding_type = bounding_area->getType();
    if (bounding_type == 1){
        RectBoundingArea* rect_boundary = dynamic_cast<RectBoundingArea*>(bounding_area.get());
        rect_boundary->draw();
    }
    else if (bounding_type == 2){
        CircleBoundingArea* circle_boundary = dynamic_cast<CircleBoundingArea*>(bounding_area.get());
        circle_boundary->draw(5000);
    }
}

void Solver::startUpdateThread(){
    update_thread_running = true;
    update_thread = std::thread(&Solver::updateLoop, this);
}

void Solver::updateLoop(){
    while (update_thread_running){
        update();
    }
}

void Solver::update() {
    if (objects.empty()){
        return;
    }

    updateGrid();
    const float substep_dt = step_dt / substeps;

    for (int i = 0; i < substeps; ++i) {
        if (gravity.x != 0 && gravity.y != 0) {
            execInParallel([this](size_t start, size_t end) { applyGravity(start, end); });
        }

        execInParallel([this, substep_dt](size_t start, size_t end) { updateObjects(substep_dt, start, end); });

        execInParallel([this](size_t start, size_t end) { checkAllParticleCollisions(start, end); });

        if (bounding_area) {
            execInParallel([this](size_t start, size_t end) { applyBoundary(start, end); });
        }
    }
}

void Solver::addBoundary(std::unique_ptr<BoundingArea> boundary){
    bounding_area = std::move(boundary);
}

std::unique_ptr<BoundingArea>& Solver::getBoundary(){
    return bounding_area;
}

std::vector<Particle>& Solver::getObjects(){
    return objects;
}

float Solver::getStepdt(){
    return step_dt;
}

int Solver::getSubsteps(){
    return substeps;
}

void Solver::setObjectVelocity(Particle& obj, glm::vec2 v){
    obj.setVelocity(v, step_dt);
}

void Solver::setGravity(glm::vec2 g){
    gravity = g;
}

void Solver::setStepDt(float dt){
    step_dt = dt;
}

void Solver::setSubsteps(int substeps_){
    substeps = substeps_;
}

void Solver::mousePull(glm::vec2 pos){
    for (auto& obj : objects){
        glm::vec2 dir = pos - obj.position;
        float dist = glm::length(dir);
        glm::vec2 a = dir * std::max(0.0f, 3 * (120 - dist));
        obj.accelerate(a);
    }
}

void Solver::applyGravity(size_t start, size_t end) {
    for (size_t i = start; i < end; ++i) {
        objects[i].accelerate(gravity);
    }
}

void Solver::applyBoundary(size_t start, size_t end) {
    const int boundary_type = bounding_area->getType();
    
    for (size_t i = start; i < end; ++i) {
        auto& obj = objects[i];
        glm::vec2 velocity = obj.getVelocity(); 
        
        if(boundary_type == 1){
            RectBoundingArea* rect_boundary = dynamic_cast<RectBoundingArea*>(bounding_area.get());

            if (obj.position.y - obj.radius <= rect_boundary->top_line){
                obj.position.y = rect_boundary->top_line + obj.radius;
                velocity.y *= -bounce_coefficient; 
                obj.setVelocity(velocity, 1.0f);
            }
            if (obj.position.y + obj.radius > rect_boundary->bottom_line){
                obj.position.y = rect_boundary->bottom_line - obj.radius;
                velocity.y *= -bounce_coefficient; 
                obj.setVelocity(velocity, 1.0f);
            }
            if (obj.position.x - obj.radius < rect_boundary->left_side){
                obj.position.x = rect_boundary->left_side + obj.radius;
                velocity.x *= -bounce_coefficient;
                obj.setVelocity(velocity, 1.0f);
            }
            if (obj.position.x + obj.radius > rect_boundary->right_side){
                obj.position.x = rect_boundary->right_side - obj.radius;
                velocity.x *= -bounce_coefficient;
                obj.setVelocity(velocity, 1.0f);
            }
        }
        else if (boundary_type == 2) {
            CircleBoundingArea* circle_boundary = dynamic_cast<CircleBoundingArea*>(bounding_area.get());
        
            glm::vec2 to_particle = obj.position - circle_boundary->center;
            float dist_from_center = glm::length(to_particle);
        
            if (dist_from_center > circle_boundary->radius - obj.radius) {
                glm::vec2 normal = to_particle / dist_from_center;
                glm::vec2 velocity = obj.getVelocity();
        
                float velocity_normal = glm::dot(velocity, normal);
        
                if (velocity_normal > 0) {
                    velocity -= (1.0f + bounce_coefficient) * velocity_normal * normal;
                }
        
                obj.position = circle_boundary->center + normal * (circle_boundary->radius - obj.radius);
                obj.setVelocity(velocity, 1.0f);
            }
        }
    }
}

void Solver::updateObjects(float dt, size_t start, size_t end) {
    for (size_t i = start; i < end; ++i) {
        objects[i].updatePos(dt);
    }
}

void Solver::execInParallel(std::function<void(size_t, size_t)> func) {
    const size_t num_threads = thread_pool.getNumThreads();
    const size_t num_objects = objects.size();
    const size_t min_chunk_size = 50;
    const size_t chunk_size = (num_objects + num_threads - 1) / num_threads; // Ensure at least one object per chunk

    if (chunk_size < min_chunk_size) {
        // If chunk size is less than 50, process all objects on a single thread
        thread_pool.enqueue([this, func, num_objects] { func(0, num_objects); });
    }
    else
    {
        for (size_t t = 0; t < num_threads; ++t) {
            size_t start = t * chunk_size;
            size_t end = (t == num_threads - 1) ? objects.size() : (t + 1) * chunk_size;
            thread_pool.enqueue([this, func, start, end] { func(start, end); });
        }
    }
    
    thread_pool.wait_for_tasks();
}

void Solver::updateGrid() {
    cell_map.clear();
    for (auto& obj: objects){
        auto cell = getCell(obj.position);
        cell_map[cell].push_back(&obj);
    }
}

std::pair<int, int> Solver::getCell(const glm::vec2& pos) const {
    int x = static_cast<int>(pos.x / cell_size);
    int y = static_cast<int>(pos.y / cell_size);
    return {x, y};
}

void Solver::checkNeighbouringCells(const std::pair<int, int>& cell){
    static const std::vector<std::pair<int, int>> neighbours = {
        {0, 1}, {1, 0}, {1, 1}, {1, -1}
    };
    const auto& particles = cell_map[cell];
    for (const auto& offset : neighbours) {
        std::pair<int, int> neighbour_cell = {cell.first + offset.first, cell.second + offset.second};
        if (cell_map.find(neighbour_cell) != cell_map.end()){
            const auto& neighbour_particles = cell_map[neighbour_cell];
            for (auto& obj : particles){
                for (auto& other : neighbour_particles){
                    checkOneParticleCollision(*obj, *other);
                }
            }
        }
    }
}

void Solver::checkOneParticleCollision(Particle& obj, Particle& other){
    const glm::vec2 d_vec = obj.position - other.position;
    const float dist = glm::length(d_vec);
    const float min_dist = obj.radius + other.radius;
    if (dist < min_dist){
        glm::vec2 n = d_vec / dist;
        const float total_mass = obj.mass + other.mass;
        const float mass_ratio = obj.mass / total_mass;
        const float delta = 0.5f * (min_dist - dist);

        obj.position += n * (1 - mass_ratio) * delta;
        other.position -= n * mass_ratio * delta;
    }
}  

void Solver::checkAllParticleCollisions(size_t start, size_t end) {
    for (size_t i = start; i < end; ++i) {
        for (size_t j = i + 1; j < objects.size(); ++j) {
            checkOneParticleCollision(objects[i], objects[j]);
        }
    }
}




