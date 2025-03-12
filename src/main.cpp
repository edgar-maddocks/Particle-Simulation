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

const float SCREEN_WIDTH = 1200.0f;
const float SCREEN_HEIGHT = 800.0f;

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

            top_left_vertex = glm::vec2({offset_width, offset_height});
            top_right_vertex = glm::vec2({SCREEN_WIDTH - offset_width, offset_height});

            bottom_left_vertex = glm::vec2({offset_width, SCREEN_HEIGHT - offset_height});
            bottom_right_vertex = glm::vec2({SCREEN_WIDTH - offset_width, SCREEN_HEIGHT - offset_height});
        }

        int getType(){
            return 1;
        }

        void draw(){
            glColor3f(0.0f, 0.0f, 0.0f);
            glBegin(GL_TRIANGLES);
            glVertex2f(top_left_vertex.x, top_left_vertex.y);
            glVertex2f(top_right_vertex.x, top_right_vertex.y);
            glVertex2f(bottom_left_vertex.x, bottom_left_vertex.y);
            glVertex2f(bottom_left_vertex.x, bottom_left_vertex.y);
            glVertex2f(bottom_right_vertex.x, bottom_right_vertex.y);
            glVertex2f(top_right_vertex.x, top_right_vertex.y);
            glEnd();
        }

        private:
            glm::vec2 top_left_vertex;
            glm::vec2 top_right_vertex;
            glm::vec2 bottom_left_vertex;
            glm::vec2 bottom_right_vertex;
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

        void draw(const int num_segments){
            glColor3f(0.0f, 0.0f, 0.0f);
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(center.x, center.y);
            
            for (int i = 0; i <= num_segments; ++i) {
                float angle = 2.0f * 3.14159265359f * (float(i) / num_segments);
                float x = center.x + cos(angle) * radius;
                float y = center.y + sin(angle) * radius;
                glVertex2f(x, y);
            }
        
            glEnd();
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
            glm::vec2 a = dir * std::max(0.0f, 0.75f * (120 - dist));
            obj.accelerate(a);
        }
    }

    float generateRandom(const float max, const float min){
        return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(max - min)));
    }

    private:
    std::vector<Particle> objects;
    glm::vec2 gravity = glm::vec2({0.0f, -9.81f});
    float step_dt = 1.0f / 60;
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

int main() {
    GLFWwindow* window = StartGLFW();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    Solver solver;
    const int MAX_OBJECTS = 200;
    const float SPAWN_DELAY = 0.005f;
    const glm::vec2 SPAWN_POSITION = glm::vec2({SCREEN_WIDTH/2.0f, SCREEN_HEIGHT - 100});
    const float SPAWN_VELOCITY = 50.0f;
    const float MIN_RADIUS = 7.0f;
    const float MAX_RADIUS = 10.0f;
    const float MAX_ANGLE = 3.14159265359 * 0.5f;

    float last_time = glfwGetTime();
    float last_spawn_time = last_time;

    solver.addBoundary(std::make_unique<CircleBoundingArea>(glm::vec2({SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f}), 400.0f));

    while (!glfwWindowShouldClose(window)) {
        float current_time = glfwGetTime();
        float delta_time = current_time - last_time;
        last_time = current_time;

        if (solver.getObjects().size() < MAX_OBJECTS && current_time - last_spawn_time >= SPAWN_DELAY){
            last_spawn_time = current_time;
            float radius = solver.generateRandom(MIN_RADIUS, MAX_RADIUS);
            auto& object = solver.addObject(SPAWN_POSITION, radius);
            
            float angle = MAX_ANGLE * sin(3.0f*current_time);
            solver.setObjectVelocity(object, SPAWN_VELOCITY * glm::vec2({sin(angle), -cos(angle)}));
        }

        glClear(GL_COLOR_BUFFER_BIT);

        int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        if (state == GLFW_PRESS)
        {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            solver.mousePull(glm::vec2({xpos, SCREEN_HEIGHT - ypos}));
        }
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
