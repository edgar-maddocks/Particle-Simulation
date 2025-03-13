#define GLM_ENABLE_EXPERIMENTAL
#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include <GLFW/glfw3.h>
#include <GL/GL.h>
#include <glm/glm.hpp>

#include "../consts.cpp"

class Particle {
    public:
        glm::vec2 position;
        glm::vec2 position_last;
        glm::vec2 acceleration;
        float radius = 10.0f;
        float mass = 100.0f;

        Particle(const glm::vec2& position_, float radius_)
            : position(position_)
            , position_last(position_)
            , acceleration({0.0f, 0.0f})
            , radius(radius_)
            , mass(radius_*radius_)
            {}

        void updatePos(float dt){
            glm::vec2 displacement = position - position_last;
            position_last = position;
            position = position + displacement + acceleration * (dt * dt);
            acceleration = {};
        }

        void accelerate(const glm::vec2& a){
            acceleration += a;
        }

        void setVelocity(glm::vec2 v, float dt){
            position_last = position - (v * dt);
        }

        void addVelocity(glm::vec2 v, float dt){
            position_last -= v * dt;
        }

        glm::vec2 getVelocity(){
            return position - position_last;
        }

        void draw(int num_segments) {
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
};

#endif