#pragma once
#include <flectron/physics/vector.hpp>
#include <flectron/physics/aabb.hpp>
#include <flectron/renderer/color.hpp>
#include <flectron/renderer/renderer.hpp>
#include <flectron/renderer/animation.hpp>
#include <entt/entt.hpp>
#include <vector>
#include <array>

namespace flectron 
{

  class Entity;

  enum BodyType
  {
    Circle, Box
  };

  enum WindingOrder 
  { 
    Invalid, Clockwise, CounterClockwise 
  };

  struct TagComponent
  {
    std::string tag;

    TagComponent(const std::string& tag);
  };

  struct PositionComponent
  {
    Vector position;
    float rotation;
    entt::registry* registry;

    PositionComponent(entt::registry* registry);
    PositionComponent(entt::registry* registry, const Vector& position, float rotation);

    void rotate(float amount);
    void move(const Vector& amount);
    void moveTo(const Vector& position);
  };

  struct VertexComponent
  {
    BodyType bodyType;
    float radius;
    float width;
    float height;

    std::vector<Vector> vertices;
    std::vector<size_t> triangles;

    std::vector<Vector> transformedVertices;
    bool isTransformUpdateRequired; // TODO maybe use on_update from EnTT

    AABB aabb;
    bool isAABBUpdateRequired;

    VertexComponent(BodyType bodyType, float radius);
    VertexComponent(BodyType bodyType, float width, float height);

    const std::vector<Vector>& getTransformedVertices(const PositionComponent& pc);
    const AABB& getAABB(const PositionComponent& pc);
  };

  struct PhysicsComponent
  {
    Vector linearVelocity;
    float rotationalVelocity;

    Vector force;
    bool isStatic;

    float density;
    float mass;
    float invMass;
    float resitution;
    float area;

    PhysicsComponent(VertexComponent& vc, float density, float resitution, bool isStatic);

    void update(PositionComponent& pc, float deltaTime, const Vector& gravity);
    void addForce(const Vector& force);
  };

  struct SpatialHashGridComponent
  {
    std::pair<std::pair<int,int>, std::pair<int,int>> clientIndices;
    int clientQuery;

    SpatialHashGridComponent(const std::pair<std::pair<int,int>, std::pair<int,int>>& clientIndices, int clientQuery);
  };

  struct FillComponent
  {
    Color fillColor;
    FillComponent();
    FillComponent(const Color& fillColor);
  };

  struct StrokeComponent
  {
    Color strokeColor;
    float strokeWidth;
    StrokeComponent();
    StrokeComponent(const Color& strokeColor, float strokeWidth);
  };

  struct TextureComponent
  {
    GLuint textureIndex;
    glm::vec4 texturePositions;

    TextureComponent();
    TextureComponent(GLuint textureIndex, const glm::vec4& texturePositions);
    TextureComponent(const Ref<Texture>& texture);
    TextureComponent(const Ref<TextureAtlas>& textureAtlas, float x, float y, float width, float height);
  };

  struct TextureVertexComponent
  {
    Vector textureOffset;
    Vector textureHalfSize;
    std::array<Vector, 4> textureVertices;
    bool isTextureUpdateRequired;

    TextureVertexComponent(const Vector& offset, const Vector& size);

    const std::array<Vector, 4>& getTransformedVertices(const PositionComponent& pc);
  };

  struct AnimationComponent
  {
    Ref<AnimationAtlas> animationAtlas;
    AnimationState animationState;

    AnimationComponent();
    AnimationComponent(const Ref<AnimationAtlas>& animationAtlas, const std::string& animationState);

    void play(const std::string& name, bool reset = false);
  };

  struct LightComponent
  {
    float lightRadius;
    Color lightColor;

    LightComponent();
    LightComponent(float lightRadius, const Color& lightColor);
  };

  struct PermanentComponent
  {
    bool isPersistent{ true };
  };

  struct ScriptComponent
  {
    std::function<void()> callback;
    int order;

    ScriptComponent(std::function<void()> callback, int order = 0);
  };

}