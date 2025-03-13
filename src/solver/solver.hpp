#define GLM_ENABLE_EXPERIMENTAL
#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <vector>
#include <memory>
#include <algorithm>
#include <unordered_map>
#include <GLFW/glfw3.h>
#include <GL/GL.h>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "../particle/particle.hpp"
#include "../boundaries/boundaries.hpp"

struct pair_hash {
    template <class T1, class T2>
    std::size_t operator() (const std::pair<T1, T2>& p) const {
        auto hash1 = std::hash<T1>{}(p.first);
        auto hash2 = std::hash<T2>{}(p.second);
        return hash1 ^ (hash2 << 1); // or use boost::hash_combine
    }
};

class Solver{
    public:
    Solver() = default;

    Particle& addObject(glm::vec2 position, float radius){
        Particle new_particle = Particle(position, radius);
        return objects.emplace_back(new_particle);
    }

    void render(){
        if (bounding_area){
            renderBoundary();
        }
        renderObjects();
    }

    void update(){
        updateGrid();
        const float substep_dt = step_dt / substeps;
        for (int i = 0; i < substeps; ++i){
            applyGravity();
            updateObjects(substep_dt);
            checkAllParticleCollisions();
            if (bounding_area){
                applyBoundary();
            }
            
        }
    }

    float getStepDt(){
        return step_dt;
    }

    int getSubsteps(){
        return substeps;
    }

    std::unique_ptr<BoundingArea>& getBoundary(){
        return bounding_area;
    }

    void updateObjects(float dt){
        for (auto& obj : objects){
            obj.updatePos(dt);
        }
    }

    void checkAllParticleCollisions(){
        updateGrid();
        for (const auto& cell : cell_map){
            const auto& particles = cell.second;
            for (size_t i = 0; i < particles.size(); ++i){
                for (size_t j = i + 1; j < particles.size(); ++j) {
                    checkOneParticleCollision(*particles[i], *particles[j]);
                }
            }
            checkNeighbouringCells(cell.first);
        }
    }

    void addBoundary(std::unique_ptr<BoundingArea> boundary) {
        bounding_area = std::move(boundary);
    }

    std::vector<Particle>& getObjects(){
        return objects;
    }

    void setObjectVelocity(Particle& obj, glm::vec2 v){
        obj.setVelocity(v, step_dt);
    }

    void mousePull(glm::vec2 pos){
        for (auto& obj : objects){
            glm::vec2 dir = pos - obj.position;
            float dist = glm::length(dir);
            glm::vec2 a = dir * std::max(0.0f, (120 - dist));
            obj.accelerate(a);
        }
    }

    void renderBoundary(){
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

    void setGravity(glm::vec2 g){
        gravity = g;
    }

    void setStepDt(float dt){
        step_dt = dt;
    }

    void setSubsteps(int substeps_){
        substeps = substeps_;
    }

    private:
    std::vector<Particle> objects;
    glm::vec2 gravity = glm::vec2({0.0f, -9.81f});
    float step_dt = 1.0f / 60.0f;
    int substeps = 8;

    float bounce_coefficient = 0.9f;

    std::unique_ptr<BoundingArea> bounding_area;

    float cell_size = 50.0f;
    std::unordered_map<std::pair<int, int>, std::vector<Particle*>, pair_hash> cell_map;

    void applyGravity(){
        for (auto& obj : objects){
            obj.accelerate(gravity);
        }
    }

    void applyBoundary(){
        const int boundary_type = bounding_area->getType();
        
        for (auto& obj : objects){
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

    void updateGrid() {
        cell_map.clear();
        for (auto& obj: objects){
            auto cell = getCell(obj.position);
            cell_map[cell].push_back(&obj);
        }
    }

    std::pair<int, int> getCell(const glm::vec2& pos) const {
        int x = static_cast<int>(pos.x / cell_size);
        int y = static_cast<int>(pos.y / cell_size);
        return {x, y};
    }

    void checkNeighbouringCells(const std::pair<int, int>& cell){
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

    void checkOneParticleCollision(Particle& obj, Particle& other){
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

    void renderObjects(){
        for (auto& obj : objects){
            obj.draw(100);
        }
    }
};

#endif