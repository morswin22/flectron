#pragma once

#include <flectron/physics/vector.hpp>
#include <flectron/scene/components.hpp>
#include <vector>

namespace flectron
{

  bool collide(PositionComponent& pcA, VertexComponent& vcA, PositionComponent& pcB, VertexComponent& vcB, Vector& normal, float& depth);
  void resolveCollision(PhysicsComponent& phcA, PhysicsComponent& phcB, const Vector& normal);

  bool intersectCircles(const Vector& centerA, float radiusA, const Vector& centerB, float radiusB, Vector& normal, float& depth);

  bool intersectPolygons(const std::vector<Vector>& verticesA, const std::vector<Vector>& verticesB, Vector& normal, float& depth);
  bool intersectPolygons(const Vector& centerA, const std::vector<Vector>& verticesA, const Vector& centerB, const std::vector<Vector>& verticesB, Vector& normal, float& depth);

  bool intersectCirclePolygon(const Vector& center, float radius, const std::vector<Vector>& vertices, Vector& normal, float& depth);
  bool intersectCirclePolygon(const Vector& center, float radius, const Vector& polygonCenter, const std::vector<Vector>& vertices, Vector& normal, float& depth);

  int findClosestPointOnPolygon(const Vector& circleCenter, const std::vector<Vector>& vertices);

  void projectCircle(const Vector& center, float radius, const Vector& axis, float& min, float& max);

  void projectVertices(const std::vector<Vector>& vertices, const Vector& axis, float& min, float& max);

  Vector findArithmeticMean(const std::vector<Vector>& vertices);

}
