#pragma once

#include <MindlessEngine/vector.hpp>
#include <vector>

namespace MindlessEngine 
{

  enum BodyType
  {
    Circle, Box
  };

  class Body 
  {
  private:
    Vector position;
    Vector linearVelocity;

    float rotation;
    float rotationalVelocity;

  public:
    float density;
    float mass;
    float resitution;
    float area;

    bool isStatic;

  private:
    int numVertices;
    Vector* vertices;
    int* triangles;
    Vector* transformedVertices;
    bool isTransformUpdateRequired;

  public:
    int getNumVertices() const;
    Vector* getTransformedVertices();
    int* getTriangles() const;

    BodyType bodyType;

    // TODO store only the propper body type values
    float radius;
    float width;
    float height;

  private:
    Body(const Vector& position, float density, float mass, float resitution, float area, bool isStatic, BodyType bodyType, float radius, float width, float height);

  public:
    ~Body();

    void rotate(float amount);

    friend Body createCircleBody(float radius, const Vector& position, float density, float resitution, bool isStatic);
    friend Body createBoxBody(float width, float height, const Vector& position, float density, float resitution, bool isStatic);
  };

  Body createCircleBody(float radius, const Vector& position, float density, float resitution, bool isStatic);
  Body createBoxBody(float width, float height, const Vector& position, float density, float resitution, bool isStatic);

  Vector* createBoxVertices(float width, float height);
  Vector* createCircleVertices(float radius);

  int* trianglesFromVertices(Vector* vertices, int numVertices);

};
