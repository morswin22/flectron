#pragma once

#include <MindlessEngine/vector.hpp>

namespace MindlessEngine
{

  struct Transform
  {
    float x;
    float y;
    float sin;
    float cos;

    Transform();
    Transform(float x, float y, float angle);
    Transform(Vector position, float angle);
  };

  Vector transform(const Vector& v, const Transform& transform);

};
