#include <glm/glm.hpp>

#include "constants.hpp"

const float GraphicsConstants::SCREEN_WIDTH = 1200.0f;
const float GraphicsConstants::SCREEN_HEIGHT = 800.0f;

const int SolverConstants::MAX_OBJECTS = 5000;
const float SolverConstants::SPAWN_DELAY = 0.05f;
const glm::vec2 SolverConstants::SolverConstants::SPAWN_POSITION = glm::vec2({GraphicsConstants::GraphicsConstants::SCREEN_WIDTH/2.0f, GraphicsConstants::GraphicsConstants::SCREEN_HEIGHT  - 100});
const float SolverConstants::SolverConstants::SPAWN_VELOCITY = 5.0f;