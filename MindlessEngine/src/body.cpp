#include <MindlessEngine/body.hpp>
#include <MindlessEngine/world.hpp>
#include <MindlessEngine/math.hpp>
#include <MindlessEngine/transform.hpp>

#include <math.h>
#include <exception>
#include <iostream>

namespace MindlessEngine 
{

  Body::Body(const Vector& position, float density, float mass, float resitution, float area, bool isStatic, BodyType bodyType, float radius, float width, float height)
    : position(position), linearVelocity(), rotation(0.0f), rotationalVelocity(0.0f), force(), 
      density(density), mass(mass), invMass(0.0f), resitution(resitution), area(area), isStatic(isStatic), 
      numVertices(0), vertices(nullptr), triangles(nullptr), transformedVertices(nullptr), isTransformUpdateRequired(true),
      bodyType(bodyType), radius(radius), width(width), height(height), 
      fillColor(Colors::lightPurple()), strokeColor(Colors::white()), textureIndex(0), texturePositions(0.0f, 0.0f, 0.0f, 0.0f),
      isFilled(true), isStroked(false), isTextured(false)
  {
    if (!isStatic)
    {
      invMass = 1.0f / mass;
    }
    else
    {
      invMass = 0.0f;
    }

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
    transformedVertices = new Vector[numVertices];

    int numTriangles = numVertices - 2;
    triangles = trianglesFromVertices(vertices, numVertices);
  }

  Body::~Body()
  {
    delete[] vertices;
    delete[] triangles;
    delete[] transformedVertices;
  }

  Body::Body(const Body& other)
    : Body(other.position, other.density, other.mass, other.resitution, other.area, other.isStatic, other.bodyType, other.radius, other.width, other.height)
  {
    linearVelocity = other.linearVelocity;
    rotation = other.rotation;
    rotationalVelocity = other.rotationalVelocity;
    force = other.force;

    invMass = other.invMass;

    numVertices = other.numVertices;
    vertices = new Vector[numVertices];
    for (int i = 0; i < numVertices; i++)
      vertices[i] = other.vertices[i];

    transformedVertices = new Vector[numVertices];
    for (int i = 0; i < numVertices; i++)
      transformedVertices[i] = other.transformedVertices[i];

    int numTriangles = numVertices - 2;
    triangles = trianglesFromVertices(vertices, numVertices);

    isTransformUpdateRequired = other.isTransformUpdateRequired;

    fillColor = other.fillColor;
    strokeColor = other.strokeColor;
    textureIndex = other.textureIndex;
    texturePositions = other.texturePositions;
    isFilled = other.isFilled;
    isStroked = other.isStroked;
    isTextured = other.isTextured;
  }

  Body::Body(Body&& other)
    : position(other.position), linearVelocity(other.linearVelocity), rotation(other.rotation), rotationalVelocity(other.rotationalVelocity), 
      force(other.force), density(other.density), mass(other.mass), invMass(other.invMass), resitution(other.resitution), area(other.area), isStatic(other.isStatic), 
      numVertices(other.numVertices), vertices(other.vertices), triangles(other.triangles), transformedVertices(other.transformedVertices), isTransformUpdateRequired(other.isTransformUpdateRequired),
      bodyType(other.bodyType), radius(other.radius), width(other.width), height(other.height), 
      fillColor(other.fillColor), strokeColor(other.strokeColor), textureIndex(other.textureIndex), texturePositions(other.texturePositions),
      isFilled(other.isFilled), isStroked(other.isStroked), isTextured(other.isTextured)
  {
    other.vertices = nullptr;
    other.triangles = nullptr;
    other.transformedVertices = nullptr;
  }

  Body& Body::operator=(Body&& other)
  {
    if (this != &other)
    {
      delete[] vertices;
      delete[] triangles;
      delete[] transformedVertices;

      position = other.position;
      linearVelocity = other.linearVelocity;
      rotation = other.rotation;
      rotationalVelocity = other.rotationalVelocity;
      force = other.force;
      density = other.density;
      mass = other.mass;
      invMass = other.invMass;
      resitution = other.resitution;
      area = other.area;
      isStatic = other.isStatic;
      numVertices = other.numVertices;
      vertices = other.vertices;
      triangles = other.triangles;
      transformedVertices = other.transformedVertices;
      isTransformUpdateRequired = other.isTransformUpdateRequired;
      bodyType = other.bodyType;
      radius = other.radius;
      width = other.width;
      height = other.height;
      fillColor = other.fillColor;
      strokeColor = other.strokeColor;
      textureIndex = other.textureIndex;
      texturePositions = other.texturePositions;
      isFilled = other.isFilled;
      isStroked = other.isStroked;
      isTextured = other.isTextured;

      other.vertices = nullptr;
      other.triangles = nullptr;
      other.transformedVertices = nullptr;
    }

    return *this;
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
        transformedVertices[i] = transform(vertices[i], tf);

      isTransformUpdateRequired = false;
    }

    return transformedVertices;
  }

  unsigned int* Body::getTriangles() const
  {
    return triangles;
  }

  void Body::fill(const Color& color)
  {
    fillColor = color;
    isFilled = true;
    isTextured = false;
  }

  void Body::noFill()
  {
    isFilled = false;
  }
  
  void Body::stroke(const Color& color)
  {
    strokeColor = color;
    isStroked = true;
  }

  void Body::noStroke()
  {
    isStroked = false;
  }

  void Body::texture(const Texture& texture)
  {
    if (numVertices != 4)
      throw std::runtime_error("Can only texture a quad");
    textureIndex = texture.get();
    texturePositions = { 0.0f, 0.0f, 1.0f, 1.0f };
    isTextured = true;
    isFilled = false;
  }

  void Body::texture(const TextureAtlas& textureAtlas, float x, float y, float width, float height)
  {
    if (numVertices != 4)
      throw std::runtime_error("Can only texture a quad");
    textureIndex = textureAtlas.get(x, y, width, height, texturePositions);
    isTextured = true;
    isFilled = false;
  }

  void Body::noTexture()
  {
    isTextured = false;
  }

  void Body::update(float deltaTime, const Vector& gravity) 
  {
    if (isStatic)
      return;

    // Vector acceleration = force / mass;
    // linearVelocity = linearVelocity + acceleration * deltaTime;

    linearVelocity = linearVelocity + gravity * deltaTime;

    // TODO look into this later
    if (length(linearVelocity) > 0.0f)
      isTransformUpdateRequired = true;

    position = position + linearVelocity * deltaTime;
    rotation += rotationalVelocity * deltaTime;

    force.x = 0.0f;
    force.y = 0.0f;
  }

  void Body::rotate(float amount)
  {
    rotation += amount;
    isTransformUpdateRequired = true;
  }

  void Body::move(const Vector& amount)
  {
    position = position + amount;
    isTransformUpdateRequired = true;
  }

  void Body::moveTo(const Vector& position)
  {
    this->position = position;
    isTransformUpdateRequired = true;
  }

  void Body::addForce(const Vector& amount)
  {
    force = force + amount;
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
    float bottom = -top;

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
    float step = 2.0f * (float)M_PI / (float)World::numCircleVerticies;
    for (int i = 0; i < World::numCircleVerticies; i++)
    {
      vertices[i].x = radius * cos(-i * step);
      vertices[i].y = radius * sin(-i * step);
    }
    return vertices;
  }

  unsigned int* trianglesFromVertices(Vector* vertices, int numVertices)
  {
    if (numVertices < 3)
      throw std::invalid_argument("Too few vertices");

    if (!isSimplePolygon(vertices, numVertices))
      throw std::invalid_argument("Not a simple polygon");
    
    if (containsColinearEdges(vertices, numVertices))
      throw std::invalid_argument("Polygon contains colinear edges");

    WindingOrder order = getWindingOrder(vertices, numVertices);
    if (order == WindingOrder::Invalid)
      throw std::invalid_argument("Invalid winding order");
    
    if (order == WindingOrder::CounterClockwise)
    {
      for (int i = 0; i < numVertices; i++)
      {
        Vector temp = vertices[i];
        vertices[i] = vertices[numVertices - 1 - i];
        vertices[numVertices - 1 - i] = temp;
      }
    }

    std::vector<unsigned int> indexList;
    for (unsigned int i = 0; i < numVertices; i++)
      indexList.push_back(i);

    unsigned int* triangles = new unsigned int[(numVertices - 2) * 3];
    int index = 0;

    while (indexList.size() > 3)
    {
      for (int i = 0; i < indexList.size(); i++)
      {
        unsigned int a = indexList[i];
        unsigned int b = indexList[getIndex(i - 1, indexList.size())];
        unsigned int c = indexList[getIndex(i + 1, indexList.size())];

        Vector va = vertices[a];
        Vector vb = vertices[b];
        Vector vc = vertices[c];

        Vector va2vb = vb - va;
        Vector va2vc = vc - va;

        if (cross(va2vb, va2vc) < 0.0f)
          continue;

        bool isEar = true;

        for (int j = 0; j < numVertices; j++)
        {
          if (j == a || j == b || j == c)
            continue;
          
          Vector p = vertices[j];

          if (isPointInTriangle(p, vb, va, vc))
          {
            isEar = false;
            break;
          }

        }
        
        if (isEar)
        {
          triangles[index++] = b;
          triangles[index++] = a;
          triangles[index++] = c;
          indexList.erase(std::next(indexList.begin(), i));
          break;
        }
      }
    }

    triangles[index++] = indexList[0];
    triangles[index++] = indexList[1];
    triangles[index++] = indexList[2];

    return triangles;
  }

  int getIndex(int index, int length)
  {
    if (index >= length)
      return index % length;
    if (index < 0) 
      return index % length + length;
    return index;
  }

  bool isPointInTriangle(const Vector& point, const Vector& a, const Vector& b, const Vector& c)
  {
    Vector ab = b - a;
    Vector bc = c - b;
    Vector ca = a - c;

    Vector ap = point - a;
    Vector bp = point - b;
    Vector cp = point - c;

    float crossA = cross(ab, ap);
    float crossB = cross(bc, bp);
    float crossC = cross(ca, cp);

    if (crossA > 0.0f || crossB > 0.0f || crossC > 0.0f)
      return false;

    return true;
  }

  WindingOrder getWindingOrder(Vector* vertices, int numVertices)
  {
    int sum{ 0 };

    for (int i = 0; i < numVertices; i++)
    {
      Vector a = vertices[i];
      Vector b = vertices[getIndex(i - 1, numVertices)];
      Vector c = vertices[getIndex(i + 1, numVertices)];

      sum += orientation(b, a, c);
    }

    if (sum > 0)
      return WindingOrder::Clockwise;

    if (sum < 0)
      return WindingOrder::CounterClockwise;

    return WindingOrder::Invalid;
  }

  int orientation(const Vector& p, const Vector& q, const Vector& r)
  {
    // See https://www.geeksforgeeks.org/orientation-3-ordered-points/
    float value = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
    if (value == 0) 
      return 0;  // colinear
    return (value > 0) ? 1 : -1; // clock or counterclock wise
  }

  bool containsColinearEdges(Vector* vertices, int numVertices)
  {
    for (int i = 0; i < numVertices; i++)
    {
      Vector a = vertices[i];
      Vector b = vertices[getIndex(i - 1, numVertices)];
      Vector c = vertices[getIndex(i + 1, numVertices)];

      if (orientation(b, a, c) == 0)
        return true;
    }

    return false;
  }

  bool isSimplePolygon(Vector* vertices, int numVertices)
  {
    for (int i = 0; i < numVertices; i++)
    {
      Vector a = vertices[i];
      Vector b = vertices[getIndex(i + 1, numVertices)];

      for (int j = i + 1; j < numVertices; j++)
      {
        Vector c = vertices[j];
        Vector d = vertices[getIndex(j + 1, numVertices)];

        if ((c.x == b.x && c.y == b.y) || (a.x == d.x && a.y == d.y))
          continue;

        if (intersectLines(a, b, c, d))
          return false;
      }
    }
    return true;
  }

  bool intersectLines(const Vector& a, const Vector& b, const Vector& c, const Vector& d)
  {
    // https://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/
    int o1 = orientation(a, b, c);
    int o2 = orientation(a, b, d);
    int o3 = orientation(c, d, a);
    int o4 = orientation(c, d, b);

    // General case
    if (o1 != o2 && o3 != o4)
        return true;

    // Special Cases
    if (o1 == 0 && isOnLineSegment(a, c, b)) return true;
    if (o2 == 0 && isOnLineSegment(a, d, b)) return true;
    if (o3 == 0 && isOnLineSegment(c, a, d)) return true;
    if (o4 == 0 && isOnLineSegment(c, b, d)) return true;

    return false; // Doesn't fall in any of the above cases
  }

  bool isOnLineSegment(const Vector& p, const Vector& q, const Vector& r)
  {
    if (q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) && q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y))
      return true;
    return false;
  }

};
