#define GLM_ENABLE_EXPERIMENTAL
#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include <glm/glm.hpp>


class Particle {
    public:
        glm::vec2 position;
        glm::vec2 position_last;
        glm::vec2 acceleration;
        float radius = 10.0f;
        float mass = 100.0f;

        Particle(const glm::vec2& position_, float radius_);

        void updatePos(float dt);

        void accelerate(const glm::vec2& a);

        void setVelocity(glm::vec2 v, float dt);

        void addVelocity(glm::vec2 v, float dt);

        glm::vec2 getVelocity();

        void draw(int num_segments);
};

#endif