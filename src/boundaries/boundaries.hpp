#define GLM_ENABLE_EXPERIMENTAL
#ifndef BOUNDARIES_HPP
#define BOUNDARIES_HPP

#include <memory>
#include <GLFW/glfw3.h>
#include <GL/GL.h>
#include <glm/glm.hpp>

#include "../consts.cpp"

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

        static std::unique_ptr<RectBoundingArea> create(const float width, const float height){
            return std::make_unique<RectBoundingArea>(width, height);
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

        static std::unique_ptr<CircleBoundingArea> create(const float center_x, const float center_y, const float radius){
            return std::make_unique<CircleBoundingArea>(glm::vec2({center_x, center_y}), radius);
        }
};

#endif