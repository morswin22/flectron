#include <MindlessEngine/body.hpp>
#include <MindlessEngine/world.hpp>
#include <MindlessEngine/math.hpp>
#include <MindlessEngine/transform.hpp>

#include <math.h>
#include <exception>

namespace MindlessEngine 
{

  Body::Body(const Vector& position, float density, float mass, float resitution, float area, bool isStatic, BodyType bodyType, float radius, float width, float height)
    : position(position), linearVelocity(), rotation(0.0f), rotationalVelocity(0.0f), density(density), mass(mass), resitution(resitution), area(area), isStatic(isStatic), numVertices(0), vertices(nullptr), triangles(nullptr), transformedVertices(nullptr), isTransformUpdateRequired(true), bodyType(bodyType), radius(radius), width(width), height(height)
  {
    if (bodyType == BodyType::Box)
    {
      numVertices = 4;
      vertices = createBoxVertices(width, height);
    }
    else if (bodyType == BodyType::Circle)
    {
      numVertices = World::numCircleVerticies;
      vertices = createCircleVertices(radius);
    }
    triangles = trianglesFromVertices(vertices, numVertices);
  }

  Body::~Body()
  {
    delete[] vertices;
    delete[] triangles;
    delete[] transformedVertices;
  }

  int Body::getNumVertices() const
  {
    return numVertices;
  }

  Vector* Body::getTransformedVertices()
  {
    if (isTransformUpdateRequired)
    {
      Transform tf(position, rotation);

      for (int i = 0; i < numVertices; i++)
      {
        transformedVertices[i] = transform(vertices[i], tf);
      }
    }

    return transformedVertices;
  }

  int* Body::getTriangles() const
  {
    return triangles;
  }

  void Body::rotate(float amount)
  {
    rotation += amount;
    isTransformUpdateRequired = true;
  }

  Body createCircleBody(float radius, const Vector& position, float density, float resitution, bool isStatic)
  {
    float area = radius * radius * (float)M_PI;

    if (area < World::minBodySize)
      throw std::invalid_argument("Body size too small");

    if (area > World::maxBodySize)
      throw std::invalid_argument("Body size too large");

    if (density < World::minBodyDensity)
      throw std::invalid_argument("Body density too small");

    if (density > World::maxBodyDensity)
      throw std::invalid_argument("Body density too large");

    resitution = clamp(resitution, 0.0f, 1.0f);

    float mass = area * density;

    return { position, density, mass, resitution, area, isStatic, BodyType::Circle, radius, 0.0f, 0.0f };
  }

  Body createBoxBody(float width, float height, const Vector& position, float density, float resitution, bool isStatic)
  {
    float area = width * height;

    if (area < World::minBodySize)
      throw std::invalid_argument("Body size too small");

    if (area > World::maxBodySize)
      throw std::invalid_argument("Body size too large");

    if (density < World::minBodyDensity)
      throw std::invalid_argument("Body density too small");

    if (density > World::maxBodyDensity)
      throw std::invalid_argument("Body density too large");

    resitution = clamp(resitution, 0.0f, 1.0f);

    float mass = area * density;

    return { position, density, mass, resitution, area, isStatic, BodyType::Box, 0.0f, width, height };
  }

  Vector* createBoxVertices(float width, float height)
  {
    float left = -width / 2.0f;
    float right = left + width;
    float top = height / 2.0f;
    float bottom = height - top;

    return new Vector[4]{
      { left, top },
      { right, top },
      { right, bottom },
      { left, bottom }
    };
  }

  Vector* createCircleVertices(float radius)
  {
    Vector* vertices = new Vector[World::numCircleVerticies];
    for (int i = 0; i < World::numCircleVerticies; i++)
    {
      float x = radius * cos(i * (float)M_PI / 180.0f);
      float y = radius * sin(i * (float)M_PI / 180.0f);
      vertices[i] = { x, y };
    }
    return vertices;
  }

  int* trianglesFromVertices(Vector* vertices, int numVertices)
  {
    // TODO implement ear clipping triangulation
    return nullptr;
  }

};
