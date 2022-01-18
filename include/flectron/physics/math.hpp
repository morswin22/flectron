#pragma once

#include <flectron/physics/vector.hpp>

namespace flectron 
{

  float clamp(float value, float min, float max);
  int clamp(int value, int min, int max);

  float length(const Vector& v);
  float lengthSquared(const Vector& v);

  float distance(const Vector& a, const Vector& b);
  float distanceSquared(const Vector& a, const Vector& b);

  Vector normalize(const Vector& v);

  float dot(const Vector& a, const Vector& b);

  float cross(const Vector& a, const Vector& b);

}
