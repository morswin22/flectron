#pragma once

#include <vector>
#include <flectron/physics/vector.hpp>

namespace flectron
{

  enum WindingOrder : short;

  std::vector<size_t> trianglesFromVertices(std::vector<Vector>& vertices);
  int getIndex(int index, int length);
  bool isPointInTriangle(const Vector& point, const Vector& a, const Vector& b, const Vector& c);
  WindingOrder getWindingOrder(const std::vector<Vector>& vertices);
  int orientation(const Vector& p, const Vector& q, const Vector& r);
  bool containsColinearEdges(const std::vector<Vector>& vertices);
  bool isSimplePolygon(const std::vector<Vector>& vertices);
  bool intersectLines(const Vector& a, const Vector& b, const Vector& c, const Vector& d);
  bool isOnLineSegment(const Vector& p, const Vector& q, const Vector& r);

  float polygonArea(const std::vector<Vector>& vertices);
  float polygonInertia(const std::vector<Vector>& vertices, float mass);

}
