#pragma once

#include <MindlessEngine/memory.hpp>
#include <MindlessEngine/vector.hpp>
#include <MindlessEngine/renderer.hpp>
#include <MindlessEngine/color.hpp>
#include <MindlessEngine/aabb.hpp>
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

    AABB aabb;
    bool isAABBUpdateRequired;

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
    GLuint textureIndex;
    glm::vec4 texturePositions;
    Vector textureOffset;
    Vector textureHalfSize;
    Vector textureVertices[4];
    bool isTextureUpdateRequired;
    bool useTextureVertices;
    Ref<AnimationAtlas> animationAtlas;
    AnimationState animationState;
    float lightRadius;
    Color lightColor;
    bool isFilled;
    bool isStroked;
    bool isTextured;
    bool isAnimated;
    bool isLit;

  public:
    Body(const Vector& position, float density, float mass, float resitution, float area, bool isStatic, BodyType bodyType, float radius, float width, float height);
    ~Body();

    Body(const Body& body) = delete;
    Body& operator=(const Body& body) = delete;

    Body(Body&& body) = delete;
    Body& operator=(Body&& body) = delete;

    void fill(const Color& color);
    void noFill();

    void stroke(const Color& color);
    void noStroke();

    void texture(const Ref<Texture>& texture);
    void texture(const Ref<TextureAtlas>& textureAtlas, float x, float y, float width, float height);
    void noTexture();

    void animation(const std::string& name, bool reset = false);
    void animation(const Ref<AnimationAtlas>& animation);
    void animation(const Ref<AnimationAtlas>& animation, const std::string& name);
    void noAnimation();

    void textureBounds(const Vector& offset, const Vector& size);
    void noTextureBounds();
    Vector* getTextureVertices();

    void light(float radius, const Color& color);
    void noLight();

    void update(float deltaTime, const Vector& gravity);

    void rotate(float amount);
    
    void move(const Vector& amount);
    void moveTo(const Vector& position);

    void addForce(const Vector& amount);

    AABB getAABB();
  };

  Ref<Body> createCircleBody(float radius, const Vector& position, float density, float resitution, bool isStatic);
  Ref<Body> createBoxBody(float width, float height, const Vector& position, float density, float resitution, bool isStatic);

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
