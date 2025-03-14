#define GLM_ENABLE_EXPERIMENTAL

#include <memory>
#include <GLFW/glfw3.h>
#include <GL/GL.h>
#include <glm/glm.hpp>

#include "../constants/constants.hpp"

#include "boundaries.hpp"


BoundingArea::~BoundingArea() = default;


RectBoundingArea::RectBoundingArea(float width, float height){
    float offset_width = (GraphicsConstants::SCREEN_WIDTH - width) / 2;
    float offset_height = (GraphicsConstants::SCREEN_HEIGHT - height) / 2;
    
    top_line = offset_height;
    bottom_line = GraphicsConstants::SCREEN_HEIGHT - offset_height;

    left_side = offset_width;
    right_side = GraphicsConstants::SCREEN_WIDTH - offset_width;

    top_left_vertex = glm::vec2({offset_width, offset_height});
    top_right_vertex = glm::vec2({GraphicsConstants::SCREEN_WIDTH - offset_width, offset_height});

    bottom_left_vertex = glm::vec2({offset_width, GraphicsConstants::SCREEN_HEIGHT - offset_height});
    bottom_right_vertex = glm::vec2({GraphicsConstants::SCREEN_WIDTH - offset_width, GraphicsConstants::SCREEN_HEIGHT - offset_height});
}

int RectBoundingArea::getType(){
    return 1;
}

void RectBoundingArea::draw(){
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

std::unique_ptr<RectBoundingArea> RectBoundingArea::create(const float width, const float height){
    return std::make_unique<RectBoundingArea>(width, height);
}    
    

        
CircleBoundingArea::CircleBoundingArea(glm::vec2 center_, float radius_) 
: center(center_), radius(radius_) {}

int CircleBoundingArea::getType(){
    return 2;
}

void CircleBoundingArea::draw(const int num_segments){
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

std::unique_ptr<CircleBoundingArea> CircleBoundingArea::create(const float center_x, const float center_y, const float radius){
    return std::make_unique<CircleBoundingArea>(glm::vec2({center_x, center_y}), radius);
}