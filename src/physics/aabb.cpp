#include <flectron/physics/aabb.hpp>
#include <flectron/renderer/renderer.hpp>

namespace flectron
{

  AABB::AABB(const Vector& min, const Vector& max)
    : min(min), max(max)
  {}

  AABB::AABB(float minX, float minY, float maxX, float maxY)
    : min(minX, minY), max(maxX, maxY)
  {}

  void AABB::render(const Color& color) const
  {
    Renderer::debugLine(min, { max.x, min.y }, color);
    Renderer::debugLine({ max.x, min.y }, max, color);
    Renderer::debugLine(max, { min.x, max.y }, color);
    Renderer::debugLine({ min.x, max.y }, min, color);
  }

}
