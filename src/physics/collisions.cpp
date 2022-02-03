#include <flectron/physics/collisions.hpp>

#include <flectron/physics/math.hpp>
#include <flectron/physics/transform.hpp>
#include <float.h>

namespace flectron
{

  Collision::Collision()
    : normal(), depth(0.0f), contact(), contacts(0u)
  {}

  bool collide(PositionComponent& pcA, VertexComponent& vcA, PositionComponent& pcB, VertexComponent& vcB, Collision& collision)
  {
    switch (vcA.shape)
    {
    case ShapeType::Circle:
      switch (vcB.shape)
      {
      case ShapeType::Circle:
        return intersectCircles(pcA.position, vcA.entity.get<CircleComponent>().radius, pcB.position, vcB.entity.get<CircleComponent>().radius, collision);
      case ShapeType::Box:
        return intersectCirclePolygon(pcA.position, vcA.entity.get<CircleComponent>().radius, pcB.position, vcB.getTransformedVertices(pcB), collision);
      case ShapeType::Polygon:
        return intersectCirclePolygon(pcA.position, vcA.entity.get<CircleComponent>().radius, transform(vcB.center, { pcB.position, pcB.rotation }), vcB.getTransformedVertices(pcB), collision);
      }
      return false;
    case ShapeType::Box:
      switch (vcB.shape)
      {
      case ShapeType::Circle:
        return intersectCirclePolygon(pcB.position, vcB.entity.get<CircleComponent>().radius, pcA.position, vcA.getTransformedVertices(pcA), collision, true);
      case ShapeType::Box:
        return intersectPolygons(pcA.position, vcA.getTransformedVertices(pcA), pcB.position, vcB.getTransformedVertices(pcB), collision);
      case ShapeType::Polygon:
        return intersectPolygons(pcA.position, vcA.getTransformedVertices(pcA), transform(vcB.center, { pcB.position, pcB.rotation }), vcB.getTransformedVertices(pcB), collision);
      }
      return false;
    case ShapeType::Polygon:
      switch (vcB.shape)
      {
      case ShapeType::Circle:
        return intersectCirclePolygon(pcB.position, vcB.entity.get<CircleComponent>().radius, transform(vcA.center, { pcA.position, pcA.rotation }), vcA.getTransformedVertices(pcA), collision, true);
      case ShapeType::Box:
        return intersectPolygons(transform(vcA.center, { pcA.position, pcA.rotation }), vcA.getTransformedVertices(pcA), pcB.position, vcB.getTransformedVertices(pcB), collision);
      case ShapeType::Polygon:
        return intersectPolygons(transform(vcA.center, { pcA.position, pcA.rotation }), vcA.getTransformedVertices(pcA), transform(vcB.center, { pcB.position, pcB.rotation }), vcB.getTransformedVertices(pcB), collision);
      }
      return false;
    }
    return false;
  }

  void resolveCollision(PhysicsComponent& phcA, PhysicsComponent& phcB, Collision& collision)
  {
    for (size_t i = 0; i < collision.contacts; i++)
    {
      Vector radiusA = collision.contact[i] - collision.centerA;
      Vector radiusB = collision.contact[i] - collision.centerB;

      Vector relativeVelocity = phcB.linearVelocity + cross(phcB.rotationalVelocity, radiusB) - phcA.linearVelocity - cross(phcA.rotationalVelocity, radiusA);
      float contactVelocity = dot(relativeVelocity, collision.normal);

      if (contactVelocity > 0.0f)
        continue;

      float e = std::min(phcA.resitution, phcB.resitution);

      float crossAN = cross(radiusA, collision.normal);
      float crossBN = cross(radiusB, collision.normal);
      float invMassSum = phcA.invMass + phcB.invMass + crossAN * crossAN * phcA.invInertia + crossBN * crossBN * phcB.invInertia;

      float j = -(1.0f + e) * contactVelocity;
      j /= invMassSum;
      j /= static_cast<float>(collision.contacts);

      Vector impulse = j * collision.normal;

      phcA.applyImpulse(-impulse, radiusA);
      phcB.applyImpulse( impulse, radiusB);

      // Friction
      relativeVelocity = phcB.linearVelocity + cross(phcB.rotationalVelocity, radiusB) - phcA.linearVelocity - cross(phcA.rotationalVelocity, radiusA);

      Vector t = relativeVelocity - (collision.normal * dot(relativeVelocity, collision.normal));

      if (dot(t, t) < FLT_EPSILON)
        continue;

      t = normalize(t);

      // j tangent magnitude
      float jt = -dot(relativeVelocity, t);
      jt /= invMassSum;
      jt /= static_cast<float>(collision.contacts);

      // Coulumb's law
      Vector tangentImpulse;
      if(std::abs(jt) < j * std::sqrt(phcA.staticFriction * phcB.staticFriction))
        tangentImpulse = t * jt;
      else
        tangentImpulse = t * -j * std::sqrt(phcA.dynamicFriction * phcB.dynamicFriction);
      
      phcA.applyImpulse(-tangentImpulse, radiusA);
      phcB.applyImpulse( tangentImpulse, radiusB);
    }
  }
  
  bool intersectCircles(const Vector& centerA, float radiusA, const Vector& centerB, float radiusB, Collision& collision)
  {
    float dist = distance(centerA, centerB);
    float radii = radiusA + radiusB;

    if (dist >= radii)
      return false;

    if (dist == 0.0f)
    {
      collision.normal = Vector(1.0f, 0.0f);
      collision.depth = radiusA;
      collision.contact[0] = centerA;
    }
    else
    {
      collision.normal = (centerB - centerA) / dist;
      collision.depth = radii - dist;
      collision.contact[0] = centerA + collision.normal * radiusA;
    }
    collision.centerA = centerA;
    collision.centerB = centerB;
    collision.contacts = 1u;

    return true;
  }

  bool intersectPolygons(const Vector& centerA, const std::vector<Vector>& verticesA, const Vector& centerB, const std::vector<Vector>& verticesB, Collision& collision)
  {
    float minA, maxA;
    float minB, maxB;

    collision.depth = FLT_MAX;

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
      if (axisDepth < collision.depth)
      {
        collision.normal = axis;
        collision.depth = axisDepth;
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
      if (axisDepth < collision.depth)
      {
        collision.normal = axis;
        collision.depth = axisDepth;
      }
    }

    Vector direction = centerB - centerA;
    if (dot(collision.normal, direction) < 0)
      collision.normal = -collision.normal;
    
    collision.centerA = centerA;
    collision.centerB = centerB;
    
    collision.contacts = 0u;
    for (int i = 0; i < verticesA.size(); i++)
    {
      const Vector& va = verticesA[i];
      const Vector& vb = verticesA[(i + 1) % verticesA.size()];

      for (int j = 0; j < verticesB.size(); j++)
      {
        const Vector& vc = verticesB[j];
        const Vector& vd = verticesB[(j + 1) % verticesB.size()];

        if (findLineLineIntersection(va, vb, vc, vd, collision.contact[collision.contacts]))
        {
          collision.contacts++;
          if (collision.contacts == 2)
            return true;
        }
      }
    }

    return true;
  }

  bool intersectCirclePolygon(const Vector& center, float radius, const Vector& polygonCenter, const std::vector<Vector>& vertices, Collision& collision, bool inverse)
  {
    float minA, maxA;
    float minB, maxB;

    collision.depth = FLT_MAX;

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
      if (axisDepth < collision.depth)
      {
        collision.normal = axis;
        collision.depth = axisDepth;
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

    if (axisDepth < collision.depth)
    {
      collision.normal = axis;
      collision.depth = axisDepth;
    }

    Vector direction = polygonCenter - center;
    if (dot(collision.normal, direction) < 0)
      collision.normal = -collision.normal;

    collision.centerA = center;
    collision.centerB = polygonCenter;
    collision.contact[0] = center + collision.normal * radius;
    collision.contacts = 1u;

    if (inverse)
    {
      collision.normal = -collision.normal;
      collision.centerA = polygonCenter;
      collision.centerB = center;
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
      std::swap(min, max);
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

  bool findLineLineIntersection(const Vector& a, const Vector& b, const Vector& c, const Vector& d, Vector& intersection)
  {
    float s1_x, s1_y, s2_x, s2_y;
    s1_x = b.x - a.x;     s1_y = b.y - a.y;
    s2_x = d.x - c.x;     s2_y = d.y - c.y;

    float s, t;
    s = (-s1_y * (a.x - c.x) + s1_x * (a.y - c.y)) / (-s2_x * s1_y + s1_x * s2_y);
    t = ( s2_x * (a.y - c.y) - s2_y * (a.x - c.x)) / (-s2_x * s1_y + s1_x * s2_y);

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
    {
      intersection = { a.x + (t * s1_x), a.y + (t * s1_y) };
      return true;
    }

    return false;
  }

}
