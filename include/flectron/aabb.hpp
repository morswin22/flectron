#include <flectron/vector.hpp>

namespace flectron
{

  struct AABB
  {
    Vector min;
    Vector max;

    AABB(const Vector& min, const Vector& max);
    AABB(float minX, float minY, float maxX, float maxY);
  };

}
