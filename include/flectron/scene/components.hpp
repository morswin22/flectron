#pragma once
#include <flectron/physics/vector.hpp>
#include <flectron/physics/aabb.hpp>
#include <flectron/renderer/color.hpp>
#include <flectron/renderer/renderer.hpp>
#include <flectron/renderer/animation.hpp>
#include <flectron/scene/entity.hpp>
#include <vector>
#include <array>

namespace flectron 
{

  enum ShapeType
  {
    Circle, Box, Polygon
  };

  enum WindingOrder 
  { 
    Invalid, Clockwise, CounterClockwise 
  };

  struct TagComponent
  {
    Entity entity;
    std::string tag;

    TagComponent(Entity entity, const std::string& tag);
  };

  struct PositionComponent
  {
    Entity entity;
    Vector position;
    float rotation;

    PositionComponent(Entity entity);
    PositionComponent(Entity entity, const Vector& position, float rotation);

    void rotate(float amount);
    void move(const Vector& amount);
    void moveTo(const Vector& position);
  };

  struct PolygonComponent
  {
    Entity entity;
    std::vector<Vector> vertices;

    PolygonComponent(Entity entity, const std::vector<Vector>& vertices);
  };

  struct BoxComponent
  {
    Entity entity;
    float width;
    float height;

    BoxComponent(Entity entity, float width, float height);
  };

  struct CircleComponent
  {
    Entity entity;
    float radius;
    float thickness = 1.0f;
    float fade = 0.005f;

    CircleComponent(Entity entity, float radius);
  };

  struct VertexComponent
  {
    Entity entity;
    ShapeType shape;

    Vector center;
    std::vector<Vector> vertices;
    std::vector<size_t> triangles;

    Vector transformedCenter;
    std::vector<Vector> transformedVertices;
    bool isTransformUpdateRequired;
    bool isTransformCenterUpdateRequired;

    AABB aabb;
    bool isAABBUpdateRequired;

    VertexComponent(Entity entity);

    const std::vector<Vector>& getTransformedVertices(const PositionComponent& pc);
    const Vector& getTransformedCenter(const PositionComponent& pc);
    const AABB& getAABB(const PositionComponent& pc);
  };

  struct PhysicsComponent
  {
    Entity entity;
    Vector linearVelocity;
    float rotationalVelocity;

    Vector force;
    float torque;
    bool isStatic;

    float density;
    float area;

    float mass;
    float invMass;
    
    float inertia;
    float invInertia;

    float resitution;

    float staticFriction;
    float dynamicFriction;

    PhysicsComponent(Entity entity, float density, float resitution, bool isStatic);

    void update(PositionComponent& pc, float deltaTime, const Vector& gravity);

    void applyForce(const Vector& force);
    void applyTorque(float torque);
    void applyImpulse(const Vector& impulse, const Vector& offset);
  };

  struct SpatialHashGridComponent
  {
    Entity entity;
    std::pair<std::pair<int,int>, std::pair<int,int>> clientIndices;
    int clientQuery;

    SpatialHashGridComponent(Entity entity, const std::pair<std::pair<int,int>, std::pair<int,int>>& clientIndices, int clientQuery);
  };

  struct FillComponent
  {
    Entity entity;
    Color fillColor;
    FillComponent(Entity entity);
    FillComponent(Entity entity, const Color& fillColor);
  };

  struct StrokeComponent
  {
    Entity entity;
    Color strokeColor;
    float strokeWidth;
    StrokeComponent(Entity entity);
    StrokeComponent(Entity entity, const Color& strokeColor, float strokeWidth);
  };

  struct TextureComponent
  {
    Entity entity;
    GLuint textureIndex;
    glm::vec4 texturePositions;

    TextureComponent(Entity entity);
    TextureComponent(Entity entity, GLuint textureIndex, const glm::vec4& texturePositions);
    TextureComponent(Entity entity, const Ref<Texture>& texture);
    TextureComponent(Entity entity, const Ref<TextureAtlas>& textureAtlas, float x, float y, float width, float height);
  };

  struct TextureVertexComponent
  {
    Entity entity;
    Vector textureOffset;
    Vector textureHalfSize;
    std::array<Vector, 4> textureVertices;
    bool isTextureUpdateRequired;

    TextureVertexComponent(Entity entity, const Vector& offset, const Vector& size);

    const std::array<Vector, 4>& getTransformedVertices(const PositionComponent& pc);
  };

  struct AnimationComponent
  {
    Entity entity;
    Ref<AnimationAtlas> animationAtlas;
    AnimationState animationState;

    AnimationComponent(Entity entity);
    AnimationComponent(Entity entity, const Ref<AnimationAtlas>& animationAtlas, const std::string& animationState);

    void play(const std::string& name, bool reset = false);
  };

  struct LightComponent
  {
    Entity entity;
    float lightRadius;
    Color lightColor;

    LightComponent(Entity entity);
    LightComponent(Entity entity, float lightRadius, const Color& lightColor);
  };

  struct TemporaryComponent
  {
    Entity entity;
    TemporaryComponent(Entity entity);
  };

  struct ScriptComponent
  {
    Entity entity;
    std::function<void()> callback;
    int order;

    ScriptComponent(Entity entity, std::function<void()> callback, int order = 0);
  };

}