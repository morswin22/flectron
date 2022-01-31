#pragma once

#include <flectron/physics/vector.hpp>
#include <flectron/scene/components.hpp>
#include <vector>

namespace flectron
{

  struct Collision
  {
    Vector normal;
    float depth;
    Vector centerA;
    Vector centerB;
    Vector contact[2]; // TODO should it be allowed to have more than 2 contacts?
    size_t contacts;

    Collision();
  };

  bool collide(PositionComponent& pcA, VertexComponent& vcA, PositionComponent& pcB, VertexComponent& vcB, Collision& collision);
  void resolveCollision(PhysicsComponent& phcA, PhysicsComponent& phcB, Collision& collision);

  bool intersectCircles(const Vector& centerA, float radiusA, const Vector& centerB, float radiusB, Collision& collision);

  bool intersectPolygons(const std::vector<Vector>& verticesA, const std::vector<Vector>& verticesB, Collision& collision);
  bool intersectPolygons(const Vector& centerA, const std::vector<Vector>& verticesA, const Vector& centerB, const std::vector<Vector>& verticesB, Collision& collision);

  bool intersectCirclePolygon(const Vector& center, float radius, const std::vector<Vector>& vertices, Collision& collision);
  bool intersectCirclePolygon(const Vector& center, float radius, const Vector& polygonCenter, const std::vector<Vector>& vertices, Collision& collision);

  int findClosestPointOnPolygon(const Vector& circleCenter, const std::vector<Vector>& vertices);

  void projectCircle(const Vector& center, float radius, const Vector& axis, float& min, float& max);

  void projectVertices(const std::vector<Vector>& vertices, const Vector& axis, float& min, float& max);

  Vector findArithmeticMean(const std::vector<Vector>& vertices);

  bool findLineLineIntersection(const Vector& pointA, const Vector& directionA, const Vector& pointB, const Vector& directionB, Vector& intersection);

}
