#define GLM_ENABLE_EXPERIMENTAL

#include <iostream>
#include <vector>
#include <GLFW/glfw3.h>
#include <GL/GL.h>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>


GLFWwindow* StartGLFW();

const float SCREEN_WIDTH = 800.0f;
const float SCREEN_HEIGHT = 600.0f;
const float GRAVITY = -9.81 / 20.0f;

class Particle {
    public:
        glm::vec2 position;
        glm::vec2 velocity;
        float radius;

        Particle(const glm::vec2& pos, const glm::vec2& vel, float r)
            : position(pos), velocity(vel), radius(r) {}

        void accelerate(float x, float y) {
            velocity.x += x;
            velocity.y += y;
        }

        void updatePos(float deltaTime) {
            position += velocity * deltaTime * 60.0f;  // Scale for ~60 FPS
        }

        void handleScreenBoundaryCollisions() {
            if (position.y - radius < 0) {
                position.y = radius;
                velocity.y *= -0.8f;
            }
            if (position.y + radius > SCREEN_HEIGHT) {
                position.y = SCREEN_HEIGHT - radius;
                velocity.y *= -0.8f;
            }
            if (position.x - radius < 0) {
                position.x = radius;
                velocity.x *= -0.8f;
            }
            if (position.x + radius > SCREEN_WIDTH) {
                position.x = SCREEN_WIDTH - radius;
                velocity.x *= -0.8f;
            }
        }

        void handleParticleCollision(Particle& other){
            glm::vec2 diff = other.position - position;
            float dSquared = glm::length2(diff);
            float sumR = radius + other.radius;

            if (dSquared < sumR * sumR){
                float d = sqrt(dSquared);
                glm::vec2 unitDCollision = diff / d;

                glm::vec2 relativeV = velocity - other.velocity;

                float vAngle = glm::dot(relativeV, unitDCollision);
                if (vAngle > 0) return;  // Ignore if moving apart

                velocity -= unitDCollision * vAngle;
                other.velocity += unitDCollision * vAngle;

                // Push particles apart to prevent overlap
                float overlap = (sumR - d) / 2.0f;
                position -= unitDCollision * overlap;
                other.position += unitDCollision * overlap;
            }
        }
};

void DrawCircle(float centerX, float centerY, float radius, int numSegments) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(centerX, centerY);

    for (int i = 0; i <= numSegments; ++i) {
        float angle = 2.0f * 3.14159265359f * (float(i) / numSegments);
        float x = centerX + cos(angle) * radius;
        float y = centerY + sin(angle) * radius;
        glVertex2f(x, y);
    }

    glEnd();
}

int main() {
    GLFWwindow* window = StartGLFW();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    std::vector<Particle> particles = {
        Particle({400.0f, 500.0f}, {1.0f, 0.0f}, 30.0f),
        Particle({300.0f, 200.0f}, {-1.0f, 2.0f}, 40.0f)
    };

    float lastTime = glfwGetTime(); 

    while (!glfwWindowShouldClose(window)) {
        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        glClear(GL_COLOR_BUFFER_BIT);

        for (auto& particle : particles) {
            particle.accelerate(0.0f, GRAVITY * deltaTime * 60.0f); 
            particle.updatePos(deltaTime);
            particle.handleScreenBoundaryCollisions();
            DrawCircle(particle.position.x, particle.position.y, particle.radius, 100);
        }

         for (size_t i = 0; i < particles.size(); i++) {
            for (size_t j = i + 1; j < particles.size(); j++) {
                particles[i].handleParticleCollision(particles[j]);
            }
        }

        for (auto& particle : particles) {
            DrawCircle(particle.position.x, particle.position.y, particle.radius, 100);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

GLFWwindow* StartGLFW() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return nullptr;
    }
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL Particles", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    return window;
}
