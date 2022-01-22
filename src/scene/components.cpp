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

  VertexComponent::VertexComponent(BodyType bodyType, float radius)
    : VertexComponent(bodyType, radius, radius)
  {}

  VertexComponent::VertexComponent(BodyType bodyType, float width, float height)
    : bodyType(bodyType), radius(width), width(width), height(height), isTransformUpdateRequired(true), aabb(0.0f, 0.0f, 0.0f, 0.0f), isAABBUpdateRequired(true)
  {
    if (bodyType == BodyType::Box)
    {
      float left = -width / 2.0f;
      float right = left + width;
      float top = height / 2.0f;
      float bottom = -top;

      vertices.push_back({ left, top });
      vertices.push_back({ right, top });
      vertices.push_back({ right, bottom });
      vertices.push_back({ left, bottom });

      transformedVertices.resize(vertices.size());

      triangles = trianglesFromVertices(vertices);
    }
    else if (bodyType == BodyType::Circle)
    {
      float step = 2.0f * (float)M_PI / (float)Scene::numCircleVerticies;
      for (int i = 0; i < Scene::numCircleVerticies; i++)
        vertices.push_back({ radius * cos(-i * step), radius * sin(-i * step) });
      
      transformedVertices.resize(vertices.size());

      triangles = trianglesFromVertices(vertices);
    }
    else
    {
      throw std::runtime_error("Unknown body type");
    }
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

    if (bodyType == BodyType::Box)
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
    else if (bodyType == BodyType::Circle)
    {
      aabb.min.x = pc.position.x - radius;
      aabb.min.y = pc.position.y - radius;
      aabb.max.x = pc.position.x + radius;
      aabb.max.y = pc.position.y + radius;
    }
    else
      throw std::runtime_error("Unsupported body type");

    isAABBUpdateRequired = false;
    return aabb;
  }

  PhysicsComponent::PhysicsComponent(VertexComponent& vc, float density, float resitution, bool isStatic)
    : linearVelocity(), rotationalVelocity(0.0f), force(), isStatic(isStatic), density(density), invMass(0.0f), resitution(clamp(resitution, 0.0f, 1.0f)), area(0.0f)
  { 
    if (vc.bodyType == BodyType::Box)
      area = vc.width * vc.height;
    else if (vc.bodyType == BodyType::Circle)
      area = (float)M_PI * vc.width * vc.width;

    if (area < Scene::minBodySize)
      throw std::invalid_argument("Body size too small");

    if (area > Scene::maxBodySize)
      throw std::invalid_argument("Body size too large");

    if (density < Scene::minBodyDensity)
      throw std::invalid_argument("Body density too small");

    if (density > Scene::maxBodyDensity)
      throw std::invalid_argument("Body density too large");

    mass = area * density;

    if (!isStatic)
      invMass = 1.0f / mass;
  }

  void PhysicsComponent::update(PositionComponent& pc, float deltaTime, const Vector& gravity)
  {
    if (isStatic)
      return;

    // Vector acceleration = force / mass;
    // linearVelocity = linearVelocity + acceleration * deltaTime;

    linearVelocity = linearVelocity + gravity * deltaTime;

    if (length(linearVelocity) > 0.0f || std::abs(rotationalVelocity) > 0.0f)
      pc.registry->patch<PositionComponent>(entt::to_entity(*pc.registry, *this));

    pc.position = pc.position + linearVelocity * deltaTime;
    pc.rotation += rotationalVelocity * deltaTime;

    force.x = 0.0f;
    force.y = 0.0f;
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

}