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

  void AABB::render(float width, const Color& color) const
  {
    Renderer::line(min, { max.x, min.y }, width, color);
    Renderer::line({ max.x, min.y }, max, width, color);
    Renderer::line(max, { min.x, max.y }, width, color);
    Renderer::line({ min.x, max.y }, min, width, color);
  }

}
