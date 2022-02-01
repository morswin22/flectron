#include <flectron/scene/components.hpp>
#include <flectron/physics/math.hpp>
#include <flectron/physics/transform.hpp>
#include <flectron/scene/scene.hpp>
#include <flectron/scene/entity.hpp>
#include <flectron/utils/vertex.hpp>
#include <cmath>

namespace flectron
{

  TagComponent::TagComponent(const std::string& tag)
    : tag(tag) 
  {}
  
  PositionComponent::PositionComponent(entt::registry* registry)
    : registry(registry), position(), rotation(0.0f)
  {}

  PositionComponent::PositionComponent(entt::registry* registry, const Vector& position, float rotation)
    : registry(registry), position(position), rotation(rotation)
  {}

  void PositionComponent::rotate(float amount)
  {
    rotation += amount;
    registry->patch<PositionComponent>(entt::to_entity(*registry, *this));
  }

  void PositionComponent::move(const Vector& amount)
  {
    position = position + amount;
    registry->patch<PositionComponent>(entt::to_entity(*registry, *this));
  }

  void PositionComponent::moveTo(const Vector& destination)
  {
    position = destination;
    registry->patch<PositionComponent>(entt::to_entity(*registry, *this));
  }

  PolygonComponent::PolygonComponent(const std::vector<Vector>& vertices)
    : vertices(vertices)
  {}

  BoxComponent::BoxComponent(float width, float height)
    : width(width), height(height)
  {}

  CircleComponent::CircleComponent(float radius)
    : radius(radius)
  {}

  VertexComponent::VertexComponent(entt::registry* registry, entt::entity entity)
    : registry(registry), entity(entity), isTransformUpdateRequired(true), aabb(0.0f, 0.0f, 0.0f, 0.0f), isAABBUpdateRequired(true)
  {
    if (registry->any_of<PolygonComponent>(entity))
    {
      vertices = registry->get<PolygonComponent>(entity).vertices;
    }
    else if (registry->any_of<BoxComponent>(entity))
    {
      auto& bc = registry->get<BoxComponent>(entity);
      vertices.push_back({ -bc.width * 0.5f,  bc.height * 0.5f });
      vertices.push_back({  bc.width * 0.5f,  bc.height * 0.5f });
      vertices.push_back({  bc.width * 0.5f, -bc.height * 0.5f });
      vertices.push_back({ -bc.width * 0.5f, -bc.height * 0.5f });
    }
    else if (registry->any_of<CircleComponent>(entity))
    {
      auto& cc = registry->get<CircleComponent>(entity);
      float step = 2.0f * (float)M_PI / (float)Scene::numCircleVerticies;
      for (int i = 0; i < Scene::numCircleVerticies; i++)
        vertices.push_back({ cc.radius * cos(-i * step), cc.radius * sin(-i * step) });
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

    if (registry->any_of<CircleComponent>(entity))
    {
      auto& cc = registry->get<CircleComponent>(entity);
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

  PhysicsComponent::PhysicsComponent(VertexComponent& vc, float density, float resitution, bool isStatic)
    : linearVelocity(), rotationalVelocity(0.0f), 
      force(), torque(0.0f), isStatic(isStatic),
      density(density), area(0.0f),
      mass(0.0f), invMass(0.0f), inertia(0.0f), invInertia(0.0f),
      staticFriction(0.4f), dynamicFriction(0.3f),
      resitution(clamp(resitution, 0.0f, 1.0f))
  { 
    if (vc.registry->any_of<BoxComponent>(vc.entity))
    {
      auto& bc = vc.registry->get<BoxComponent>(vc.entity);
      area = bc.width * bc.height;
    }
    else if (vc.registry->any_of<CircleComponent>(vc.entity))
    {
      auto& cc = vc.registry->get<CircleComponent>(vc.entity);
      area = (float)M_PI * cc.radius * cc.radius;
    }
    else
      area = polygonArea(vc.vertices);

    if (area < Scene::minBodySize)
      throw std::invalid_argument("Body size too small");

    if (area > Scene::maxBodySize)
      throw std::invalid_argument("Body size too large");

    if (density < Scene::minBodyDensity)
      throw std::invalid_argument("Body density too small");

    if (density > Scene::maxBodyDensity)
      throw std::invalid_argument("Body density too large");

    mass = area * density;

    if (vc.registry->any_of<BoxComponent>(vc.entity))
    {
      auto& bc = vc.registry->get<BoxComponent>(vc.entity);
      inertia = bc.width * bc.height * mass / 6.0f;
    }
    else if (vc.registry->any_of<CircleComponent>(vc.entity))
    {
      auto& cc = vc.registry->get<CircleComponent>(vc.entity);
      inertia = mass * cc.radius * cc.radius;
    }
    else
      inertia = polygonInertia(vc.vertices, mass);

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
      pc.registry->patch<PositionComponent>(entt::to_entity(*pc.registry, *this));

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

  SpatialHashGridComponent::SpatialHashGridComponent(const std::pair<std::pair<int,int>, std::pair<int,int>>& clientIndices, int clientQuery)
    : clientIndices(clientIndices), clientQuery(clientQuery)
  {}

  FillComponent::FillComponent()
    : fillColor(Colors::white())
  {}

  FillComponent::FillComponent(const Color& fillColor)
    : fillColor(fillColor)
  {}

  StrokeComponent::StrokeComponent()
    : strokeColor(Colors::white()), strokeWidth(1.0f)
  {}

  StrokeComponent::StrokeComponent(const Color& strokeColor, float strokeWidth)
    : strokeColor(strokeColor), strokeWidth(strokeWidth)
  {}

  TextureComponent::TextureComponent()
    : textureIndex(0), texturePositions(0.0f, 0.0f, 0.0f, 0.0f)
  {}

  TextureComponent::TextureComponent(GLuint textureIndex, const glm::vec4& texturePositions)
    : textureIndex(textureIndex), texturePositions(texturePositions)
  {}

  TextureComponent::TextureComponent(const Ref<Texture>& texture)
    : textureIndex(texture->get()), texturePositions(0.0f, 0.0f, 1.0f, 1.0f)
  {}

  TextureComponent::TextureComponent(const Ref<TextureAtlas>& textureAtlas, float x, float y, float width, float height)
  {
    textureIndex = textureAtlas->get(x, y, width, height, texturePositions);
  }

  TextureVertexComponent::TextureVertexComponent(const Vector& offset, const Vector& size)
    : textureOffset(offset), textureHalfSize(size), textureVertices(), isTextureUpdateRequired(true)
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

  AnimationComponent::AnimationComponent()
    : animationAtlas(nullptr), animationState("")
  {}

  AnimationComponent::AnimationComponent(const Ref<AnimationAtlas>& animationAtlas, const std::string& animationState)
    : animationAtlas(animationAtlas), animationState(animationState)
  {}

  void AnimationComponent::play(const std::string& name, bool reset)
  {
    if (animationState.currentName == name && !reset)
      return;
    animationState = AnimationState(name);
  }

  LightComponent::LightComponent()
    : lightRadius(0.0f), lightColor(Colors::white())
  {}

  LightComponent::LightComponent(float lightRadius, const Color& lightColor)
    : lightRadius(lightRadius), lightColor(lightColor)
  {}

  ScriptComponent::ScriptComponent(std::function<void()> callback, int order)
    : callback(callback), order(order)
  {}

}