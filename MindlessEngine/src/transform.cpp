#include <MindlessEngine/transform.hpp>

#include <math.h>

namespace MindlessEngine
{

  Transform::Transform() : x(0.0f), y(0.0f), sin(0.0f), cos(0.0f) {}

  Transform::Transform(float x, float y, float angle) : x(x), y(y), sin(sinf(angle)), cos(sinf(angle)) {}

  Transform::Transform(Vector position, float angle) : Transform(position.x, position.y, angle) {}

};