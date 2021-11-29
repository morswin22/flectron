#include <flectron/transform.hpp>

#include <math.h>

namespace flectron
{

  Transform::Transform() : x(0.0f), y(0.0f), sin(0.0f), cos(0.0f) {}

  Transform::Transform(float x, float y, float angle) : x(x), y(y), sin(sinf(angle)), cos(cosf(angle)) {}

  Transform::Transform(Vector position, float angle) : Transform(position.x, position.y, angle) {}

  Vector transform(const Vector& v, const Transform& transform)
  {
    return { 
      transform.cos * v.x - transform.sin * v.y + transform.x, 
      transform.sin * v.x + transform.cos * v.y + transform.y
    };
  }

}
