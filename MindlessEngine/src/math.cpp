#include <MindlessEngine/math.hpp>
#include <MindlessEngine/vector.hpp>

#include <math.h>

namespace MindlessEngine 
{

  float length(const Vector& v)
  {
    return sqrtf(v.x * v.x + v.y * v.y);
  }

  float lengthSquared(const Vector& v)
  {
    return v.x * v.x + v.y * v.y;
  }

  float distance(const Vector& a, const Vector& b)
  {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return sqrtf(dx * dx + dy * dy);
  }

  float distanceSquared(const Vector& a, const Vector& b)
  {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return dx * dx + dy * dy;
  }

  Vector normalize(const Vector& v)
  {
    float len = length(v);
    return { v.x / len, v.y / len };
  }

  float dot(const Vector& a, const Vector& b)
  {
    return a.x * b.x + a.y * b.y;
  }

  float cross(const Vector& a, const Vector& b)
  {
    return a.x * b.y - a.y * b.x;
  }

};
