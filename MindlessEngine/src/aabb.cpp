#include <MindlessEngine/aabb.hpp>

namespace MindlessEngine
{

  AABB::AABB(const Vector& min, const Vector& max)
    : min(min), max(max)
  {}

  AABB::AABB(float minX, float minY, float maxX, float maxY)
    : min(minX, minY), max(maxX, maxY)
  {}

};
