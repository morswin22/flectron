#include <flectron/scene/components.hpp>
#include <flectron/physics/math.hpp>
#include <flectron/physics/transform.hpp>
#include <flectron/scene/scene.hpp>
#include <flectron/utils/vertex.hpp>
#include <flectron/physics/collisions.hpp>
#include <cmath>

namespace flectron
{

  TagComponent::TagComponent(Entity entity, const std::string& tag)
    : tag(tag) 
  {}
  
  PositionComponent::PositionComponent(Entity entity)
    : entity(entity), position(), rotation(0.0f)
  {}

  PositionComponent::PositionComponent(Entity entity, const Vector& position, float rotation)
    : entity(entity), position(position), rotation(rotation)
  {}

  void PositionComponent::rotate(float amount)
  {
    rotation += amount;
    entity.patch<PositionComponent>();
  }

  void PositionComponent::move(const Vector& amount)
  {
    position = position + amount;
    entity.patch<PositionComponent>();
  }

  void PositionComponent::moveTo(const Vector& destination)
  {
    position = destination;
    entity.patch<PositionComponent>();
  }

  PolygonComponent::PolygonComponent(Entity entity, const std::vector<Vector>& vertices)
    : entity(entity), vertices(vertices)
  {}

  BoxComponent::BoxComponent(Entity entity, float width, float height)
    : entity(entity), width(width), height(height)
  {}

  CircleComponent::CircleComponent(Entity entity, float radius)
    : entity(entity), radius(radius)
  {}

  VertexComponent::VertexComponent(Entity entity)
    : entity(entity), isTransformUpdateRequired(true), aabb(0.0f, 0.0f, 0.0f, 0.0f), isAABBUpdateRequired(true)
  {
    if (entity.has<PolygonComponent>())
    {
      vertices = entity.get<PolygonComponent>().vertices;
      shape = ShapeType::Polygon;
      center = findArithmeticMean(vertices);
    }
    else if (entity.has<BoxComponent>())
    {
      auto& bc = entity.get<BoxComponent>();
      vertices.push_back({ -bc.width * 0.5f,  bc.height * 0.5f });
      vertices.push_back({  bc.width * 0.5f,  bc.height * 0.5f });
      vertices.push_back({  bc.width * 0.5f, -bc.height * 0.5f });
      vertices.push_back({ -bc.width * 0.5f, -bc.height * 0.5f });
      shape = ShapeType::Box;
    }
    else if (entity.has<CircleComponent>())
    {
      auto& cc = entity.get<CircleComponent>();
      float step = 2.0f * (float)M_PI / (float)Scene::numCircleVerticies;
      for (int i = 0; i < Scene::numCircleVerticies; i++)
        vertices.push_back({ cc.radius * cos(-i * step), cc.radius * sin(-i * step) });
      shape = ShapeType::Circle;
    }
    else
      throw std::runtime_error("Entity must have a body defining component");
    
    transformedVertices.resize(vertices.size());
    triangles = trianglesFromVertices(vertices);
  }

  const std::vector<Vector>& VertexComponent::getTransformedVertices(const PositionComponent& pc)
  {
    if (isTransformUpdateRequired)
    {
      Transform tf(pc.position, pc.rotation);

      for (int i = 0; i < vertices.size(); i++)
        transformedVertices[i] = transform(vertices[i], tf);

      isTransformUpdateRequired = false;
    }

    return transformedVertices;
  }

  const AABB& VertexComponent::getAABB(const PositionComponent& pc)
  {
    if (!isAABBUpdateRequired)
      return aabb;

    if (entity.has<CircleComponent>())
    {
      auto& cc = entity.get<CircleComponent>();
      aabb.min.x = pc.position.x - cc.radius;
      aabb.min.y = pc.position.y - cc.radius;
      aabb.max.x = pc.position.x + cc.radius;
      aabb.max.y = pc.position.y + cc.radius;
    }
    else
    {
      aabb.min.x = FLT_MAX;
      aabb.min.y = FLT_MAX;
      aabb.max.x = -FLT_MAX;
      aabb.max.y = -FLT_MAX;

      getTransformedVertices(pc);
      for (int i = 0; i < vertices.size(); i++)
      {
        if (transformedVertices[i].x < aabb.min.x) 
          aabb.min.x = transformedVertices[i].x;
        if (transformedVertices[i].y < aabb.min.y)
          aabb.min.y = transformedVertices[i].y;
        if (transformedVertices[i].x > aabb.max.x)
          aabb.max.x = transformedVertices[i].x;
        if (transformedVertices[i].y > aabb.max.y)
          aabb.max.y = transformedVertices[i].y;
      }
    }

    isAABBUpdateRequired = false;
    return aabb;
  }

  PhysicsComponent::PhysicsComponent(Entity entity, float density, float resitution, bool isStatic)
    : entity(entity),
      linearVelocity(), rotationalVelocity(0.0f), 
      force(), torque(0.0f), isStatic(isStatic),
      density(density), area(0.0f),
      mass(0.0f), invMass(0.0f), inertia(0.0f), invInertia(0.0f),
      staticFriction(0.4f), dynamicFriction(0.3f),
      resitution(clamp(resitution, 0.0f, 1.0f))
  { 
    if (entity.has<BoxComponent>())
    {
      auto& bc = entity.get<BoxComponent>();
      area = bc.width * bc.height;
    }
    else if (entity.has<CircleComponent>())
    {
      auto& cc = entity.get<CircleComponent>();
      area = (float)M_PI * cc.radius * cc.radius;
    }
    else
      area = polygonArea(entity.get<VertexComponent>().vertices);

    if (area < Scene::minBodySize)
      throw std::invalid_argument("Body size too small");

    if (area > Scene::maxBodySize)
      throw std::invalid_argument("Body size too large");

    if (density < Scene::minBodyDensity)
      throw std::invalid_argument("Body density too small");

    if (density > Scene::maxBodyDensity)
      throw std::invalid_argument("Body density too large");

    mass = area * density;

    if (entity.has<BoxComponent>())
    {
      auto& bc = entity.get<BoxComponent>();
      inertia = bc.width * bc.height * mass / 6.0f;
    }
    else if (entity.has<CircleComponent>())
    {
      auto& cc = entity.get<CircleComponent>();
      inertia = mass * cc.radius * cc.radius;
    }
    else
      inertia = polygonInertia(entity.get<VertexComponent>().vertices, mass);

    if (!isStatic)
    {
      invMass = 1.0f / mass;
      invInertia = 1.0f / inertia;
    }
  }

  void PhysicsComponent::update(PositionComponent& pc, float deltaTime, const Vector& gravity)
  {
    if (isStatic)
      return;

    linearVelocity = linearVelocity + (force * invMass + gravity) * deltaTime;
    rotationalVelocity = rotationalVelocity + (torque * invInertia) * deltaTime;

    if (dot(linearVelocity, linearVelocity) > 0.0f || rotationalVelocity != 0.0f)
      entity.patch<PositionComponent>();

    pc.position = pc.position + linearVelocity * deltaTime;
    pc.rotation += rotationalVelocity * deltaTime;

    force.x = 0.0f;
    force.y = 0.0f;
    torque = 0.0f;
  }

  void PhysicsComponent::applyForce(const Vector& force)
  {
    this->force = this->force + force;
  }

  void PhysicsComponent::applyTorque(float torque)
  {
    this->torque += torque;
  }

  void PhysicsComponent::applyImpulse(const Vector& impulse, const Vector& offset)
  {
    linearVelocity = linearVelocity + impulse * invMass;
    rotationalVelocity += cross(offset, impulse) * invInertia;
  }

  SpatialHashGridComponent::SpatialHashGridComponent(Entity entity, const std::pair<std::pair<int,int>, std::pair<int,int>>& clientIndices, int clientQuery)
    : entity(entity), clientIndices(clientIndices), clientQuery(clientQuery)
  {}

  FillComponent::FillComponent(Entity entity)
    : entity(entity), fillColor(Colors::white())
  {}

  FillComponent::FillComponent(Entity entity, const Color& fillColor)
    : entity(entity), fillColor(fillColor)
  {}

  StrokeComponent::StrokeComponent(Entity entity)
    : entity(entity), strokeColor(Colors::white()), strokeWidth(1.0f)
  {}

  StrokeComponent::StrokeComponent(Entity entity, const Color& strokeColor, float strokeWidth)
    : entity(entity), strokeColor(strokeColor), strokeWidth(strokeWidth)
  {}

  TextureComponent::TextureComponent(Entity entity)
    : entity(entity), textureIndex(0), texturePositions(0.0f, 0.0f, 0.0f, 0.0f)
  {}

  TextureComponent::TextureComponent(Entity entity, GLuint textureIndex, const glm::vec4& texturePositions)
    : entity(entity), textureIndex(textureIndex), texturePositions(texturePositions)
  {}

  TextureComponent::TextureComponent(Entity entity, const Ref<Texture>& texture)
    : entity(entity), textureIndex(texture->get()), texturePositions(0.0f, 0.0f, 1.0f, 1.0f)
  {}

  TextureComponent::TextureComponent(Entity entity, const Ref<TextureAtlas>& textureAtlas, float x, float y, float width, float height)
    : entity(entity)
  {
    textureIndex = textureAtlas->get(x, y, width, height, texturePositions);
  }

  TextureVertexComponent::TextureVertexComponent(Entity entity, const Vector& offset, const Vector& size)
    : entity(entity), textureOffset(offset), textureHalfSize(size), textureVertices(), isTextureUpdateRequired(true)
  {}

  const std::array<Vector, 4>& TextureVertexComponent::getTransformedVertices(const PositionComponent& pc)
  {
    if (isTextureUpdateRequired)
    {
      Transform tf(pc.position, pc.rotation);

      textureVertices[0] = transform({ -textureHalfSize.x + textureOffset.x,  textureHalfSize.y + textureOffset.y }, tf);
      textureVertices[1] = transform({  textureHalfSize.x + textureOffset.x,  textureHalfSize.y + textureOffset.y }, tf);
      textureVertices[2] = transform({  textureHalfSize.x + textureOffset.x, -textureHalfSize.y + textureOffset.y }, tf);
      textureVertices[3] = transform({ -textureHalfSize.x + textureOffset.x, -textureHalfSize.y + textureOffset.y }, tf);

      isTextureUpdateRequired = false;
    }

    return textureVertices;
  }

  AnimationComponent::AnimationComponent(Entity entity)
    : entity(entity), animationAtlas(nullptr), animationState("")
  {}

  AnimationComponent::AnimationComponent(Entity entity, const Ref<AnimationAtlas>& animationAtlas, const std::string& animationState)
    : entity(entity), animationAtlas(animationAtlas), animationState(animationState)
  {}

  void AnimationComponent::play(const std::string& name, bool reset)
  {
    if (animationState.currentName == name && !reset)
      return;
    animationState = AnimationState(name);
  }

  LightComponent::LightComponent(Entity entity)
    : entity(entity), lightRadius(0.0f), lightColor(Colors::white())
  {}

  LightComponent::LightComponent(Entity entity, float lightRadius, const Color& lightColor)
    : entity(entity), lightRadius(lightRadius), lightColor(lightColor)
  {}

  TemporaryComponent::TemporaryComponent(Entity entity)
    : entity(entity)
  {}

  ScriptComponent::ScriptComponent(Entity entity, std::function<void()> callback, int order)
    : entity(entity), callback(callback), order(order)
  {}

}