#define GLM_ENABLE_EXPERIMENTAL
#ifndef AIRFOIL_SIMULATOR_HPP
#define AIRFOIL_SIMULATOR_HPP

#include <vector>
#include <memory>
#include <GLFW/glfw3.h>
#include <GL/GL.h>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "../consts.cpp"
#include "../solver/solver.hpp"
#include "../particle/particle.hpp"

class AirFoilSimulator {
    public:
        AirFoilSimulator(
            const float air_speed_ = 5.0f, 
            const glm::vec2 air_direction_ = {1.0f, 0.0f}, 
            const float particle_radius_ = 10.0f
        )
        : air_speed(air_speed_)
        , air_direction(air_direction_)
        , particle_radius(particle_radius_)
        {
            solver.setGravity({0.0f, 0.0f});
            initializeAir();
        }

        void update(){
            const int substeps = solver.getSubsteps();
            const float substep_dt = solver.getStepDt() / substeps;
            for (int i = 0; i < substeps; ++i){
                solver.updateObjects(substep_dt);
                solver.checkAllParticleCollisions();
                applyBoundary();
                spawnNewParticles();
                setVelocities();
            }
        }

        void render(){
            solver.render();
        }

        void setStepDt(float dt){
            solver.setStepDt(dt);
        }
        
        void setSubsteps(int substeps){
            solver.setSubsteps(substeps);
        }

    private:
        Solver solver;
        float particle_radius;
        float air_speed;
        glm::vec2 air_direction;
        

        void initializeAir(){
            const float radius_padded = particle_radius + (2.0f * particle_radius);
            const float num_particles_height = SCREEN_HEIGHT / radius_padded;

            for (int i = 0; i < 3; ++i){
                for (int j = 0; j < num_particles_height; ++j){
                    solver.addObject(glm::vec2({i * radius_padded, j * radius_padded}), particle_radius);
                }
            }

            setVelocities();
        }

        void setVelocities(){
            for (auto& obj : solver.getObjects()){
                obj.setVelocity(air_speed * air_direction, 1.0f);
            }
        }

        void applyBoundary() {
            auto& objects = solver.getObjects();
            objects.erase(
                std::remove_if(objects.begin(), objects.end(), [](const Particle& obj) {
                    return obj.position.x > SCREEN_WIDTH;
                }),
                objects.end()
            );
        }

        void spawnNewParticles() {
            const float radius_padded = particle_radius + (2.0f * particle_radius);
            const int num_particles_height = SCREEN_HEIGHT / radius_padded;
        
            auto& objects = solver.getObjects();
            for (int i = 0; i <= num_particles_height; ++i) { // Adjusted loop to include the top row
                glm::vec2 expected_position = glm::vec2(0, i * radius_padded);
                bool position_occupied = std::any_of(objects.begin(), objects.end(), [&](const Particle& obj) {
                    return glm::distance(obj.position, expected_position) < radius_padded;
                });
        
                if (!position_occupied) {
                    solver.addObject(expected_position, particle_radius);
                }
            }
        }
};

#endif