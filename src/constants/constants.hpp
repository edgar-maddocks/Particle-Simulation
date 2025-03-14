#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <glm/glm.hpp>

class GraphicsConstants {
    public:
        static const float SCREEN_WIDTH;
        static const float SCREEN_HEIGHT;
};

class SolverConstants {
    public:
        static const int MAX_OBJECTS;
        static const float SPAWN_DELAY;
        static const glm::vec2 SPAWN_POSITION;
        static const float SPAWN_VELOCITY;
};

#endif