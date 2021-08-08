#pragma once

#include <MindlessEngine/vector.hpp>
#include <MindlessEngine/renderer.hpp>
#include <MindlessEngine/color.hpp>
#include <vector>

namespace MindlessEngine 
{

  enum WindingOrder 
  { 
    Invalid, Clockwise, CounterClockwise 
  };

  enum BodyType
  {
    Circle, Box
  };

  class Body 
  {
  public:
    Vector position;
    Vector linearVelocity;

  private:
    float rotation;
    float rotationalVelocity;

    Vector force;

  public:
    float density;
    float mass;
    float invMass;
    float resitution;
    float area;

    bool isStatic;

  private:
    int numVertices;
    Vector* vertices;
    unsigned int* triangles;
    Vector* transformedVertices;
    bool isTransformUpdateRequired;

  public:
    int getNumVertices() const;
    Vector* getTransformedVertices();
    unsigned int* getTriangles() const;

    BodyType bodyType;

    // TODO store only the propper body type values
    float radius;
    float width;
    float height;

    Color fillColor;
    Color strokeColor;
    bool isFilled;
    bool isStroked;

  private:
    Body(const Vector& position, float density, float mass, float resitution, float area, bool isStatic, BodyType bodyType, float radius, float width, float height);

  public:
    ~Body();

    Body(const Body& body);
    Body& operator=(const Body& body) = delete;

    Body(Body&& body);
    Body& operator=(Body&& body);

    void fill(const Color& color);
    void noFill();

    void stroke(const Color& color);
    void noStroke();

    void update(float deltaTime, const Vector& gravity);

    void rotate(float amount);
    
    void move(const Vector& amount);
    void moveTo(const Vector& position);

    void addForce(const Vector& amount);

    friend Body createCircleBody(float radius, const Vector& position, float density, float resitution, bool isStatic);
    friend Body createBoxBody(float width, float height, const Vector& position, float density, float resitution, bool isStatic);
  };

  Body createCircleBody(float radius, const Vector& position, float density, float resitution, bool isStatic);
  Body createBoxBody(float width, float height, const Vector& position, float density, float resitution, bool isStatic);

  Vector* createBoxVertices(float width, float height);
  Vector* createCircleVertices(float radius);

  unsigned int* trianglesFromVertices(Vector* vertices, int numVertices);
  int getIndex(int index, int length);

  bool isPointInTriangle(const Vector& point, const Vector& a, const Vector& b, const Vector& c);
  WindingOrder getWindingOrder(Vector* vertices, int numVertices);
  int orientation(const Vector& p, const Vector& q, const Vector& r);

  bool containsColinearEdges(Vector* vertices, int numVertices);
  
  bool isSimplePolygon(Vector* vertices, int numVertices);
  bool intersectLines(const Vector& a, const Vector& b, const Vector& c, const Vector& d);
  bool isOnLineSegment(const Vector& p, const Vector& q, const Vector& r);

};
