#define GLM_ENABLE_EXPERIMENTAL
#ifndef BOUNDARIES_HPP
#define BOUNDARIES_HPP

#include <memory>
#include <glm/glm.hpp>


struct BoundingArea {
    virtual ~BoundingArea();
    virtual int getType() = 0; // Make this a pure virtual function
};

struct RectBoundingArea : BoundingArea{
    public:
        float top_line;
        float bottom_line;
        float left_side;
        float right_side;
        
        RectBoundingArea(float width, float height);

        int getType();
        void draw();
        static std::unique_ptr<RectBoundingArea> create(const float width, const float height); 

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
        
        CircleBoundingArea(glm::vec2 center_, float radius_);

        int getType();
        void draw(const int num_segments);
        static std::unique_ptr<CircleBoundingArea> create(const float center_x, const float center_y, const float radius);
};

#endif