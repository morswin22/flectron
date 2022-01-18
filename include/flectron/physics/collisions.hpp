#pragma once

#include <flectron/physics/vector.hpp>
#include <vector>

namespace flectron
{

  bool intersectCircles(const Vector& centerA, float radiusA, const Vector& centerB, float radiusB, Vector& normal, float& depth);

  bool intersectPolygons(const std::vector<Vector>& verticesA, const std::vector<Vector>& verticesB, Vector& normal, float& depth);
  bool intersectPolygons(const Vector& centerA, const std::vector<Vector>& verticesA, const Vector& centerB, const std::vector<Vector>& verticesB, Vector& normal, float& depth);

  bool intersectCirclePolygon(const Vector& center, float radius, const std::vector<Vector>& vertices, Vector& normal, float& depth);
  bool intersectCirclePolygon(const Vector& center, float radius, const Vector& polygonCenter, const std::vector<Vector>& vertices, Vector& normal, float& depth);

  int findClosestPointOnPolygon(const Vector& circleCenter, const std::vector<Vector>& vertices);

  void projectVertices(const std::vector<Vector>& vertices, const Vector& axis, float& min, float& max);

  Vector findArithmeticMean(const std::vector<Vector>& vertices);

  bool intersectPolygons(const Vector* verticesA, int numVerticesA, const Vector* verticesB, int numVerticesB, Vector& normal, float& depth);
  bool intersectPolygons(const Vector& centerA, const Vector* verticesA, int numVerticesA, const Vector& centerB, const Vector* verticesB, int numVerticesB, Vector& normal, float& depth);

  bool intersectCirclePolygon(const Vector& center, float radius, const Vector* vertices, int numVertices, Vector& normal, float& depth);
  bool intersectCirclePolygon(const Vector& center, float radius, const Vector& polygonCenter, const Vector* vertices, int numVertices, Vector& normal, float& depth);

  int findClosestPointOnPolygon(const Vector& circleCenter, const Vector* vertices, int numVertices);

  void projectCircle(const Vector& center, float radius, const Vector& axis, float& min, float& max);

  void projectVertices(const Vector* vertices, int numVertices, const Vector& axis, float& min, float& max);

  Vector findArithmeticMean(const Vector* vertices, int numVertices);

}
