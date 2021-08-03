#pragma once

#include <MindlessEngine/vector.hpp>

namespace MindlessEngine 
{

  float clamp(float value, float min, float max);

  float length(const Vector& v);
  float lengthSquared(const Vector& v);

  float distance(const Vector& a, const Vector& b);
  float distanceSquared(const Vector& a, const Vector& b);

  Vector normalize(const Vector& v);

  float dot(const Vector& a, const Vector& b);

  float cross(const Vector& a, const Vector& b);

};
