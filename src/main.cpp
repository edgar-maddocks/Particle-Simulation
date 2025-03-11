#define GLM_ENABLE_EXPERIMENTAL

#include <iostream>
#include <random>
#include <vector>
#include <memory>
#include <GLFW/glfw3.h>
#include <GL/GL.h>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>


GLFWwindow* StartGLFW();

const float SCREEN_WIDTH = 800.0f;
const float SCREEN_HEIGHT = 600.0f;

class Particle {
    public:
        glm::vec2 position;
        glm::vec2 position_last;
        glm::vec2 acceleration;
        float radius = 10.0f;
        float mass = 100.0f;

        Particle(const glm::vec2& position_, float radius_)
            : position(position_), position_last(position_), radius(radius_) {
            }

        void updatePos(float dt){
            glm::vec2 displacement = position - position_last;
            position_last = position;
            position = position + displacement + acceleration * (dt * dt);
            acceleration = {};
        }

        void accelerate(glm::vec2 a){
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

struct BoundingArea {
    virtual ~BoundingArea() = default;
    virtual int getType() = 0; // Make this a pure virtual function
};

struct RectBoundingArea : BoundingArea{
    public:
        float top_line;
        float bottom_line;
        float left_side;
        float right_side;
        
        RectBoundingArea(float width, float height){
            float offset_width = (SCREEN_WIDTH - width) / 2;
            float offset_height = (SCREEN_HEIGHT - height) / 2;
            
            top_line = offset_height;
            bottom_line = SCREEN_HEIGHT - offset_height;

            left_side = offset_width;
            right_side = SCREEN_WIDTH - offset_width;
        }

        int getType(){
            return 1;
        }
};

struct CircleBoundingArea : BoundingArea{
    public:
        glm::vec2 center;
        float radius;
        
        CircleBoundingArea(glm::vec2 center_, float radius_) 
        : center(center_), radius(radius_) {}

        int getType(){
            return 2;
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
        for (auto& obj : objects){
            obj.draw(100);
        }
    }

    void update(){
        applyGravity();
        applyBoundary();
        updateObjects(step_dt);
    }

    void addBoundary(std::unique_ptr<BoundingArea> boundary) {
        bounding_area = std::move(boundary);
    }

    private:
    std::vector<Particle> objects;
    glm::vec2 gravity = {0.0f, -100.0f};
    float step_dt = 1.0f / 60;

    float bounce_coefficient = 0.95f;

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
            if (boundary_type == 2)
            {
                
            }
            
        }
    }

    void updateObjects(float dt){
        for (auto& obj : objects){
            obj.updatePos(dt);
        }
    }
};

int main() {
    GLFWwindow* window = StartGLFW();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    Solver solver;
    auto& object = solver.addObject({420.0f, 100.0f}, 10.0f);
    solver.addBoundary(std::make_unique<RectBoundingArea>(800.0f, 600.0f));

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        solver.update();
        solver.render();

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
