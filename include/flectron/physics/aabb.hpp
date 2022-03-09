#pragma once
#include <flectron/physics/vector.hpp>
#include <flectron/renderer/color.hpp>

namespace flectron
{

  struct AABB
  {
    Vector min;
    Vector max;

    AABB(const Vector& min, const Vector& max);
    AABB(float minX, float minY, float maxX, float maxY);

    void render(const Color& color = Colors::white()) const;
  };

}
