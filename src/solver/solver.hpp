#define GLM_ENABLE_EXPERIMENTAL
#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <vector>
#include <memory>
#include <GLFW/glfw3.h>
#include <GL/GL.h>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "../particle/particle.hpp"
#include "../boundaries/boundaries.hpp"

class Solver{
    public:
    Solver() = default;

    Particle& addObject(glm::vec2 position, float radius){
        Particle new_particle = Particle(position, radius);
        return objects.emplace_back(new_particle);
    }

    void render(){
        renderBoundary();
        renderObjects();
    }

    void update(){
        const float substep_dt = step_dt / substeps;
        for (int i = 0; i < substeps; ++i){
            applyGravity();
            updateObjects(substep_dt);
            checkAllParticleCollisions();
            applyBoundary();
        }
    }

    void addBoundary(std::unique_ptr<BoundingArea> boundary) {
        bounding_area = std::move(boundary);
    }

    std::vector<Particle> getObjects(){
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

    float generateRandom(const float max, const float min){
        return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(max - min)));
    }

    private:
    std::vector<Particle> objects;
    glm::vec2 gravity = glm::vec2({0.0f, -9.81f});
    float step_dt = 1.0f / 60.0f;
    const int substeps = 8;

    float bounce_coefficient = 0.9f;

    std::unique_ptr<BoundingArea> bounding_area;

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
            else if (boundary_type == 2)
            {
                CircleBoundingArea* circle_boundary = dynamic_cast<CircleBoundingArea*>(bounding_area.get());

                const glm::vec2 diff = circle_boundary->center - obj.position;
                const float dist_from_center = glm::length(diff);
                if (dist_from_center > circle_boundary->radius - obj.radius){
                    const glm::vec2 n = diff / dist_from_center;
                    const glm::vec2 perp = {-n.y, n.x};
                    const glm::vec2 v = obj.getVelocity();
                    obj.position = circle_boundary->center -  n * (circle_boundary->radius - obj.radius);
                    obj.setVelocity((2.0f * (v.x * perp.x + v.y * perp.y) * perp - v) * bounce_coefficient, 1.0f);
                }
            }
            
        }
    }

    void checkAllParticleCollisions(){
        const int num_objects = objects.size();
        for (int i = 0; i < num_objects; ++i){
            for (int j = i + 1; j < num_objects; ++j){
                checkOneParticleCollision(objects[i], objects[j]);
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

    void updateObjects(float dt){
        for (auto& obj : objects){
            obj.updatePos(dt);
        }
    }

    void renderObjects(){
        for (auto& obj : objects){
            obj.draw(100);
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
};

#endif