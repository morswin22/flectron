#include <flectron/physics/collisions.hpp>

#include <flectron/physics/math.hpp>
#include <float.h>

namespace flectron
{

  bool collide(PositionComponent& pcA, VertexComponent& vcA, PositionComponent& pcB, VertexComponent& vcB, Vector& normal, float& depth)
  {
    if (!vcA.registry->any_of<CircleComponent>(vcA.entity))
    {
      if (!vcB.registry->any_of<CircleComponent>(vcB.entity))
      {
        return intersectPolygons(pcA.position, vcA.getTransformedVertices(pcA), pcB.position, vcB.getTransformedVertices(pcB), normal, depth);
      }
      else if (vcB.registry->any_of<CircleComponent>(vcB.entity))
      {
        bool result = intersectCirclePolygon(pcB.position, vcB.registry->get<CircleComponent>(vcB.entity).radius, pcA.position, vcA.getTransformedVertices(pcA), normal, depth);
        if (result)
          normal = -normal;
        return result;
      }
    }
    else if (vcA.registry->any_of<CircleComponent>(vcA.entity))
    {
      if (!vcB.registry->any_of<CircleComponent>(vcB.entity))
      {
        return intersectCirclePolygon(pcA.position, vcA.registry->get<CircleComponent>(vcA.entity).radius, pcB.position, vcB.getTransformedVertices(pcB), normal, depth);
      }
      else if (vcB.registry->any_of<CircleComponent>(vcB.entity))
      {
        return intersectCircles(pcA.position, vcA.registry->get<CircleComponent>(vcA.entity).radius, pcB.position, vcB.registry->get<CircleComponent>(vcB.entity).radius, normal, depth);
      }
    }
    return false;
  }

  void resolveCollision(PhysicsComponent& phcA, PhysicsComponent& phcB, const Vector& normal)
  {
    Vector relativeVelocity = phcB.linearVelocity - phcA.linearVelocity;

    if (dot(relativeVelocity, normal) > 0.0f)
      return;

    float e = std::min(phcA.resitution, phcB.resitution);

    float j = -(1.0f + e) * dot(relativeVelocity, normal);

    j /= phcA.invMass + phcB.invMass;

    Vector impulse = j * normal;

    phcA.linearVelocity = phcA.linearVelocity - (impulse * phcA.invMass);
    phcB.linearVelocity = phcB.linearVelocity + (impulse * phcB.invMass);
  }
  
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

}
