#pragma once

#include <MindlessEngine/vector.hpp>

namespace MindlessEngine
{

  bool intersectCircles(const Vector& centerA, float radiusA, const Vector& centerB, float radiusB, Vector& normal, float& depth);

  bool intersectPolygons(const Vector* verticesA, int numVerticesA, const Vector* verticesB, int numVerticesB, Vector& normal, float& depth);
  bool intersectPolygons(const Vector& centerA, const Vector* verticesA, int numVerticesA, const Vector& centerB, const Vector* verticesB, int numVerticesB, Vector& normal, float& depth);

  bool intersectCirclePolygon(const Vector& center, float radius, const Vector* vertices, int numVertices, Vector& normal, float& depth);
  bool intersectCirclePolygon(const Vector& center, float radius, const Vector& polygonCenter, const Vector* vertices, int numVertices, Vector& normal, float& depth);

  int findClosestPointOnPolygon(const Vector& circleCenter, const Vector* vertices, int numVertices);

  void projectCircle(const Vector& center, float radius, const Vector& axis, float& min, float& max);

  void projectVertices(const Vector* vertices, int numVertices, const Vector& axis, float& min, float& max);

  Vector findArithmeticMean(const Vector* vertices, int numVertices);

};
