#include <flectron/physics/collisions.hpp>

#include <flectron/physics/math.hpp>
#include <float.h>

namespace flectron
{
  
  bool intersectCircles(const Vector& centerA, float radiusA, const Vector& centerB, float radiusB, Vector& normal, float& depth)
  {
    float dist = distance(centerA, centerB);
    float radii = radiusA + radiusB;

    if (dist >= radii)
      return false;

    normal = normalize(centerB - centerA);
    depth = radii - dist;

    return true;
  }

  bool intersectPolygons(const std::vector<Vector>& verticesA, const std::vector<Vector>& verticesB, Vector& normal, float& depth)
  {
    float minA, maxA;
    float minB, maxB;

    depth = FLT_MAX;

    for (int i = 0; i < verticesA.size(); i++)
    {
      Vector va = verticesA[i];
      Vector vb = verticesA[(i + 1) % verticesA.size()];

      Vector edge = vb - va;
      Vector axis = normalize({ -edge.y, edge.x });

      projectVertices(verticesA, axis, minA, maxA);
      projectVertices(verticesB, axis, minB, maxB);

      if (minA >= maxB || minB >= maxA)
        return false;

      float axisDepth = std::min(maxB - minA, maxA - minB);
      if (axisDepth < depth)
      {
        depth = axisDepth;
        normal = axis;
      }
    }

    for (int i = 0; i < verticesB.size(); i++)
    {
      Vector va = verticesB[i];
      Vector vb = verticesB[(i + 1) % verticesB.size()];

      Vector edge = vb - va;
      Vector axis = normalize({ -edge.y, edge.x });

      projectVertices(verticesA, axis, minA, maxA);
      projectVertices(verticesB, axis, minB, maxB);

      if (minA >= maxB || minB >= maxA)
        return false;

      float axisDepth = std::min(maxB - minA, maxA - minB);
      if (axisDepth < depth)
      {
        depth = axisDepth;
        normal = axis;
      }
    }

    Vector centerA = findArithmeticMean(verticesA);
    Vector centerB = findArithmeticMean(verticesB);

    Vector direction = centerB - centerA;
    if (dot(normal, direction) < 0)
    {
      normal = -normal;
    }

    return true;
  }

  bool intersectPolygons(const Vector& centerA, const std::vector<Vector>& verticesA, const Vector& centerB, const std::vector<Vector>& verticesB, Vector& normal, float& depth)
  {
    float minA, maxA;
    float minB, maxB;

    depth = FLT_MAX;

    for (int i = 0; i < verticesA.size(); i++)
    {
      Vector va = verticesA[i];
      Vector vb = verticesA[(i + 1) % verticesA.size()];

      Vector edge = vb - va;
      Vector axis = normalize({ -edge.y, edge.x });

      projectVertices(verticesA, axis, minA, maxA);
      projectVertices(verticesB, axis, minB, maxB);

      if (minA >= maxB || minB >= maxA)
        return false;

      float axisDepth = std::min(maxB - minA, maxA - minB);
      if (axisDepth < depth)
      {
        depth = axisDepth;
        normal = axis;
      }
    }

    for (int i = 0; i < verticesB.size(); i++)
    {
      Vector va = verticesB[i];
      Vector vb = verticesB[(i + 1) % verticesB.size()];

      Vector edge = vb - va;
      Vector axis = normalize({ -edge.y, edge.x });

      projectVertices(verticesA, axis, minA, maxA);
      projectVertices(verticesB, axis, minB, maxB);

      if (minA >= maxB || minB >= maxA)
        return false;

      float axisDepth = std::min(maxB - minA, maxA - minB);
      if (axisDepth < depth)
      {
        depth = axisDepth;
        normal = axis;
      }
    }

    Vector direction = centerB - centerA;
    if (dot(normal, direction) < 0)
    {
      normal = -normal;
    }

    return true;
  }

  bool intersectCirclePolygon(const Vector& center, float radius, const std::vector<Vector>& vertices, Vector& normal, float& depth)
  {
    float minA, maxA;
    float minB, maxB;

    depth = FLT_MAX;

    for (int i = 0; i < vertices.size(); i++)
    {
      Vector va = vertices[i];
      Vector vb = vertices[(i + 1) % vertices.size()];

      Vector edge = vb - va;
      Vector axis = normalize({ -edge.y, edge.x });

      projectVertices(vertices, axis, minA, maxA);
      projectCircle(center, radius, axis, minB, maxB);

      if (minA >= maxB || minB >= maxA)
        return false;

      float axisDepth = std::min(maxB - minA, maxA - minB);
      if (axisDepth < depth)
      {
        depth = axisDepth;
        normal = axis;
      }
    }

    int closestPointIndex = findClosestPointOnPolygon(center, vertices);
    Vector closestPoint = vertices[closestPointIndex];

    Vector axis = normalize(closestPoint - center);

    projectVertices(vertices, axis, minA, maxA);
    projectCircle(center, radius, axis, minB, maxB);

    if (minA >= maxB || minB >= maxA)
      return false;

    float axisDepth = std::min(maxB - minA, maxA - minB);

    if (axisDepth < depth)
    {
      depth = axisDepth;
      normal = axis;
    }

    Vector polygonCenter = findArithmeticMean(vertices);

    Vector direction = polygonCenter - center;
    if (dot(normal, direction) < 0)
    {
      normal = -normal;
    }

    return true;
  }

  bool intersectCirclePolygon(const Vector& center, float radius, const Vector& polygonCenter, const std::vector<Vector>& vertices, Vector& normal, float& depth)
  {
    float minA, maxA;
    float minB, maxB;

    depth = FLT_MAX;

    for (int i = 0; i < vertices.size(); i++)
    {
      Vector va = vertices[i];
      Vector vb = vertices[(i + 1) % vertices.size()];

      Vector edge = vb - va;
      Vector axis = normalize({ -edge.y, edge.x });

      projectVertices(vertices, axis, minA, maxA);
      projectCircle(center, radius, axis, minB, maxB);

      if (minA >= maxB || minB >= maxA)
        return false;

      float axisDepth = std::min(maxB - minA, maxA - minB);
      if (axisDepth < depth)
      {
        depth = axisDepth;
        normal = axis;
      }
    }

    int closestPointIndex = findClosestPointOnPolygon(center, vertices);
    Vector closestPoint = vertices[closestPointIndex];

    Vector axis = normalize(closestPoint - center);

    projectVertices(vertices, axis, minA, maxA);
    projectCircle(center, radius, axis, minB, maxB);

    if (minA >= maxB || minB >= maxA)
      return false;

    float axisDepth = std::min(maxB - minA, maxA - minB);

    if (axisDepth < depth)
    {
      depth = axisDepth;
      normal = axis;
    }

    Vector direction = polygonCenter - center;
    if (dot(normal, direction) < 0)
    {
      normal = -normal;
    }

    return true;
  }

  int findClosestPointOnPolygon(const Vector& circleCenter, const std::vector<Vector>& vertices)
  {
    int result = -1;
    float minDistance = FLT_MAX;

    for (int i = 0; i < vertices.size(); i++)
    {
      Vector v = vertices[i];
      float dist = distance(v, circleCenter);

      if (dist < minDistance)
      {
        minDistance = dist;
        result = i;
      }
    }

    return result;
  }

  void projectVertices(const std::vector<Vector>& vertices, const Vector& axis, float& min, float& max)
  {
    min = FLT_MAX;
    max = -FLT_MAX;

    for (int i = 0; i < vertices.size(); i++)
    {
      Vector v = vertices[i];
      float proj = dot(v, axis);

      if (proj < min)
        min = proj;

      if (proj > max)
        max = proj;
    }
  }

  Vector findArithmeticMean(const std::vector<Vector>& vertices)
  {
    float sumX = 0.0f;
    float sumY = 0.0f;

    for (int i = 0; i < vertices.size(); i++)
    {
      Vector v = vertices[i];
      sumX += v.x;
      sumY += v.y;
    }

    return { sumX / vertices.size(), sumY / vertices.size() };
  }

  // ---------------------------------------------------------------------------

  bool intersectPolygons(const Vector* verticesA, int numVerticesA, const Vector* verticesB, int numVerticesB, Vector& normal, float& depth)
  {
    float minA, maxA;
    float minB, maxB;

    depth = FLT_MAX;

    for (int i = 0; i < numVerticesA; i++)
    {
      Vector va = verticesA[i];
      Vector vb = verticesA[(i + 1) % numVerticesA];

      Vector edge = vb - va;
      Vector axis = normalize({ -edge.y, edge.x });

      projectVertices(verticesA, numVerticesA, axis, minA, maxA);
      projectVertices(verticesB, numVerticesB, axis, minB, maxB);

      if (minA >= maxB || minB >= maxA)
        return false;

      float axisDepth = std::min(maxB - minA, maxA - minB);
      if (axisDepth < depth)
      {
        depth = axisDepth;
        normal = axis;
      }
    }

    for (int i = 0; i < numVerticesB; i++)
    {
      Vector va = verticesB[i];
      Vector vb = verticesB[(i + 1) % numVerticesB];

      Vector edge = vb - va;
      Vector axis = normalize({ -edge.y, edge.x });

      projectVertices(verticesA, numVerticesA, axis, minA, maxA);
      projectVertices(verticesB, numVerticesB, axis, minB, maxB);

      if (minA >= maxB || minB >= maxA)
        return false;

      float axisDepth = std::min(maxB - minA, maxA - minB);
      if (axisDepth < depth)
      {
        depth = axisDepth;
        normal = axis;
      }
    }

    Vector centerA = findArithmeticMean(verticesA, numVerticesA);
    Vector centerB = findArithmeticMean(verticesB, numVerticesB);

    Vector direction = centerB - centerA;
    if (dot(normal, direction) < 0)
    {
      normal = -normal;
    }

    return true;
  }

  bool intersectPolygons(const Vector& centerA, const Vector* verticesA, int numVerticesA, const Vector& centerB, const Vector* verticesB, int numVerticesB, Vector& normal, float& depth)
  {
    float minA, maxA;
    float minB, maxB;

    depth = FLT_MAX;

    for (int i = 0; i < numVerticesA; i++)
    {
      Vector va = verticesA[i];
      Vector vb = verticesA[(i + 1) % numVerticesA];

      Vector edge = vb - va;
      Vector axis = normalize({ -edge.y, edge.x });

      projectVertices(verticesA, numVerticesA, axis, minA, maxA);
      projectVertices(verticesB, numVerticesB, axis, minB, maxB);

      if (minA >= maxB || minB >= maxA)
        return false;

      float axisDepth = std::min(maxB - minA, maxA - minB);
      if (axisDepth < depth)
      {
        depth = axisDepth;
        normal = axis;
      }
    }

    for (int i = 0; i < numVerticesB; i++)
    {
      Vector va = verticesB[i];
      Vector vb = verticesB[(i + 1) % numVerticesB];

      Vector edge = vb - va;
      Vector axis = normalize({ -edge.y, edge.x });

      projectVertices(verticesA, numVerticesA, axis, minA, maxA);
      projectVertices(verticesB, numVerticesB, axis, minB, maxB);

      if (minA >= maxB || minB >= maxA)
        return false;

      float axisDepth = std::min(maxB - minA, maxA - minB);
      if (axisDepth < depth)
      {
        depth = axisDepth;
        normal = axis;
      }
    }

    Vector direction = centerB - centerA;
    if (dot(normal, direction) < 0)
    {
      normal = -normal;
    }

    return true;
  }

  bool intersectCirclePolygon(const Vector& center, float radius, const Vector* vertices, int numVertices, Vector& normal, float& depth)
  {
    float minA, maxA;
    float minB, maxB;

    depth = FLT_MAX;

    for (int i = 0; i < numVertices; i++)
    {
      Vector va = vertices[i];
      Vector vb = vertices[(i + 1) % numVertices];

      Vector edge = vb - va;
      Vector axis = normalize({ -edge.y, edge.x });

      projectVertices(vertices, numVertices, axis, minA, maxA);
      projectCircle(center, radius, axis, minB, maxB);

      if (minA >= maxB || minB >= maxA)
        return false;

      float axisDepth = std::min(maxB - minA, maxA - minB);
      if (axisDepth < depth)
      {
        depth = axisDepth;
        normal = axis;
      }
    }

    int closestPointIndex = findClosestPointOnPolygon(center, vertices, numVertices);
    Vector closestPoint = vertices[closestPointIndex];

    Vector axis = normalize(closestPoint - center);

    projectVertices(vertices, numVertices, axis, minA, maxA);
    projectCircle(center, radius, axis, minB, maxB);

    if (minA >= maxB || minB >= maxA)
      return false;

    float axisDepth = std::min(maxB - minA, maxA - minB);

    if (axisDepth < depth)
    {
      depth = axisDepth;
      normal = axis;
    }

    Vector polygonCenter = findArithmeticMean(vertices, numVertices);

    Vector direction = polygonCenter - center;
    if (dot(normal, direction) < 0)
    {
      normal = -normal;
    }

    return true;
  }

  bool intersectCirclePolygon(const Vector& center, float radius, const Vector& polygonCenter, const Vector* vertices, int numVertices, Vector& normal, float& depth)
  {
    float minA, maxA;
    float minB, maxB;

    depth = FLT_MAX;

    for (int i = 0; i < numVertices; i++)
    {
      Vector va = vertices[i];
      Vector vb = vertices[(i + 1) % numVertices];

      Vector edge = vb - va;
      Vector axis = normalize({ -edge.y, edge.x });

      projectVertices(vertices, numVertices, axis, minA, maxA);
      projectCircle(center, radius, axis, minB, maxB);

      if (minA >= maxB || minB >= maxA)
        return false;

      float axisDepth = std::min(maxB - minA, maxA - minB);
      if (axisDepth < depth)
      {
        depth = axisDepth;
        normal = axis;
      }
    }

    int closestPointIndex = findClosestPointOnPolygon(center, vertices, numVertices);
    Vector closestPoint = vertices[closestPointIndex];

    Vector axis = normalize(closestPoint - center);

    projectVertices(vertices, numVertices, axis, minA, maxA);
    projectCircle(center, radius, axis, minB, maxB);

    if (minA >= maxB || minB >= maxA)
      return false;

    float axisDepth = std::min(maxB - minA, maxA - minB);

    if (axisDepth < depth)
    {
      depth = axisDepth;
      normal = axis;
    }

    Vector direction = polygonCenter - center;
    if (dot(normal, direction) < 0)
    {
      normal = -normal;
    }

    return true;
  }

  int findClosestPointOnPolygon(const Vector& circleCenter, const Vector* vertices, int numVertices)
  {
    int result = -1;
    float minDistance = FLT_MAX;

    for (int i = 0; i < numVertices; i++)
    {
      Vector v = vertices[i];
      float dist = distance(v, circleCenter);

      if (dist < minDistance)
      {
        minDistance = dist;
        result = i;
      }
    }

    return result;
  }

  void projectCircle(const Vector& center, float radius, const Vector& axis, float& min, float& max)
  {
    Vector direction = normalize(axis);
    Vector directionAndRadius = direction * radius;

    Vector p1 = center + directionAndRadius;
    Vector p2 = center - directionAndRadius;

    min = dot(p1, axis);
    max = dot(p2, axis);

    if (min > max)
    {
      float tmp = min;
      min = max;
      max = tmp;
    }
  }

  void projectVertices(const Vector* vertices, int numVertices, const Vector& axis, float& min, float& max)
  {
    min = FLT_MAX;
    max = -FLT_MAX;

    for (int i = 0; i < numVertices; i++)
    {
      Vector v = vertices[i];
      float proj = dot(v, axis);

      if (proj < min)
        min = proj;

      if (proj > max)
        max = proj;
    }
  }

  Vector findArithmeticMean(const Vector* vertices, int numVertices)
  {
    float sumX = 0.0f;
    float sumY = 0.0f;

    for (int i = 0; i < numVertices; i++)
    {
      Vector v = vertices[i];
      sumX += v.x;
      sumY += v.y;
    }

    return { sumX / numVertices, sumY / numVertices };
  }

}
