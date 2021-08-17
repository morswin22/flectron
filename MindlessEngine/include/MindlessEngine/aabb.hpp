#include <MindlessEngine/vector.hpp>

namespace MindlessEngine
{

  struct AABB
  {
    Vector min;
    Vector max;

    AABB(const Vector& min, const Vector& max);
    AABB(float minX, float minY, float maxX, float maxY);
  };

};
