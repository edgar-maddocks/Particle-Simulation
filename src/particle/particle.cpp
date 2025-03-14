#define GLM_ENABLE_EXPERIMENTAL

#include <GLFW/glfw3.h>
#include <GL/GL.h>
#include <glm/glm.hpp>

#include "../constants/constants.hpp"

#include "particle.hpp"

Particle::Particle(const glm::vec2& position_, float radius_)
    : position(position_)
    , position_last(position_)
    , acceleration({0.0f, 0.0f})
    , radius(radius_)
    , mass(radius_*radius_)
    {}

void Particle::updatePos(float dt){
    glm::vec2 displacement = position - position_last;
    position_last = position;
    position = position + displacement + acceleration * (dt * dt);
    acceleration = {};
}

void Particle::accelerate(const glm::vec2& a){
    acceleration += a;
}

void Particle::setVelocity(glm::vec2 v, float dt){
    position_last = position - (v * dt);
}

void Particle::addVelocity(glm::vec2 v, float dt){
    position_last -= v * dt;
}

glm::vec2 Particle::getVelocity(){
    return position - position_last;
}

void Particle::draw(int num_segments) {
    glColor3f(0.0f, 1.0f, 1.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(position.x, position.y);

    for (int i = 0; i <= num_segments; ++i) {
        float angle = 2.0f * 3.14159265359f * (float(i) / num_segments);
        float x = position.x + cos(angle) * radius;
        float y = position.y + sin(angle) * radius;
        glVertex2f(x, y);
    }

    glEnd();
} 