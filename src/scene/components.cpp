#include <flectron/scene/components.hpp>
#include <flectron/physics/math.hpp>
#include <flectron/physics/transform.hpp>
#include <flectron/scene/scene.hpp>
#include <flectron/scene/entity.hpp>
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

  // Static VertexComponent members

  std::vector<size_t> VertexComponent::trianglesFromVertices(std::vector<Vector>& vertices)
  {
    if (vertices.size() < 3)
      throw std::invalid_argument("Too few vertices");

    if (!isSimplePolygon(vertices))
      throw std::invalid_argument("Not a simple polygon");
    
    if (containsColinearEdges(vertices))
      throw std::invalid_argument("Polygon contains colinear edges");

    WindingOrder order = getWindingOrder(vertices);
    if (order == WindingOrder::Invalid)
      throw std::invalid_argument("Invalid winding order");
    
    if (order == WindingOrder::CounterClockwise)
      for (size_t i = 0; i < vertices.size(); i++)
        std::swap(vertices[i], vertices[vertices.size() - 1 - i]);

    std::vector<size_t> indexList;
    for (size_t i = 0; i < vertices.size(); i++)
      indexList.push_back(i);

    std::vector<size_t> triangles;
    triangles.resize((vertices.size() - 2) * 3);
    int index = 0;

    while (indexList.size() > 3)
    {
      for (size_t i = 0; i < indexList.size(); i++)
      {
        size_t a = indexList[i];
        size_t b = indexList[getIndex((int)i - 1, (int)indexList.size())];
        size_t c = indexList[getIndex((int)i + 1, (int)indexList.size())];

        Vector va = vertices[a];
        Vector vb = vertices[b];
        Vector vc = vertices[c];

        Vector va2vb = vb - va;
        Vector va2vc = vc - va;

        if (cross(va2vb, va2vc) < 0.0f)
          continue;

        bool isEar = true;

        for (size_t j = 0; j < vertices.size(); j++)
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

  int VertexComponent::getIndex(int index, int length)
  {
    if (index >= length)
      return index % length;
    if (index < 0) 
      return index % length + length;
    return index;
  }

  bool VertexComponent::isPointInTriangle(const Vector& point, const Vector& a, const Vector& b, const Vector& c)
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

  WindingOrder VertexComponent::getWindingOrder(const std::vector<Vector>& vertices)
  {
    int sum{ 0 };

    for (int i = 0; i < vertices.size(); i++)
    {
      Vector a = vertices[i];
      Vector b = vertices[getIndex(i - 1, (int)vertices.size())];
      Vector c = vertices[getIndex(i + 1, (int)vertices.size())];

      sum += orientation(b, a, c);
    }

    if (sum > 0)
      return WindingOrder::Clockwise;

    if (sum < 0)
      return WindingOrder::CounterClockwise;

    return WindingOrder::Invalid;
  }

  int VertexComponent::orientation(const Vector& p, const Vector& q, const Vector& r)
  {
    // See https://www.geeksforgeeks.org/orientation-3-ordered-points/
    float value = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
    if (value == 0) 
      return 0;  // colinear
    return (value > 0) ? 1 : -1; // clock or counterclock wise
  }

  bool VertexComponent::containsColinearEdges(const std::vector<Vector>& vertices)
  {
    for (int i = 0; i < vertices.size(); i++)
    {
      Vector a = vertices[i];
      Vector b = vertices[getIndex(i - 1, (int)vertices.size())];
      Vector c = vertices[getIndex(i + 1, (int)vertices.size())];

      if (orientation(b, a, c) == 0)
        return true;
    }

    return false;
  }

  bool VertexComponent::isSimplePolygon(const std::vector<Vector>& vertices)
  {
    for (int i = 0; i < vertices.size(); i++)
    {
      Vector a = vertices[i];
      Vector b = vertices[getIndex(i + 1, (int)vertices.size())];

      for (int j = i + 1; j < vertices.size(); j++)
      {
        Vector c = vertices[j];
        Vector d = vertices[getIndex(j + 1, (int)vertices.size())];

        if ((c.x == b.x && c.y == b.y) || (a.x == d.x && a.y == d.y))
          continue;

        if (intersectLines(a, b, c, d))
          return false;
      }
    }
    return true;
  }

  bool VertexComponent::intersectLines(const Vector& a, const Vector& b, const Vector& c, const Vector& d)
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

  bool VertexComponent::isOnLineSegment(const Vector& p, const Vector& q, const Vector& r)
  {
    if (q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) && q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y))
      return true;
    return false;
  }

}