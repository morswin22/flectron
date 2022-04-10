#pragma once

#include <flectron/physics/vector.hpp>

namespace flectron 
{

  float length(const Vector& v);
  float lengthSquared(const Vector& v);

  float distance(const Vector& a, const Vector& b);
  float distanceSquared(const Vector& a, const Vector& b);

  Vector normalize(const Vector& v);

  float dot(const Vector& a, const Vector& b);

  float cross(const Vector& a, const Vector& b);
  Vector cross(const Vector& a, float b);
  Vector cross(float a, const Vector& b);

  template<typename T>
  T clamp(T value, T min, T max)
  {
    if (min == max)
      return min;

    FLECTRON_ASSERT(min < max, "min must be less than max");

    if (value < min)
      return min;

    if (value > max)
      return max;

    return value;
  }

}
