#ifndef CONSTS_CPP
#define CONSTS_CPP

#include <glm/glm.hpp>

const float SCREEN_WIDTH = 1200.0f;
const float SCREEN_HEIGHT = 800.0f;
const int MAX_OBJECTS = 500;
const float SPAWN_DELAY = 0.05f;
const glm::vec2 SPAWN_POSITION = glm::vec2({SCREEN_WIDTH/2.0f, SCREEN_HEIGHT - 100});
const float SPAWN_VELOCITY = 5.0f;

#endif