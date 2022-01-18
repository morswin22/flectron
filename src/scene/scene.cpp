#include <flectron/scene/scene.hpp>
#include <flectron/scene/entity.hpp>
#include <flectron/scene/components.hpp>
#include <flectron/physics/math.hpp>
#include <flectron/physics/collisions.hpp>
#include <flectron/utils/profile.hpp>

namespace flectron 
{

  Environment::Environment()
    : nightColor(Colors::darkGray()), darkness(0.0f)
  {}

  Environment::Environment(const Color& nightColor, float darkness)
    : nightColor(nightColor), darkness(darkness)
  {}

  float Scene::minBodySize = 0.01f * 0.01f;
  float Scene::maxBodySize = 64.0f * 64.0f;

  float Scene::minBodyDensity = 0.5f; // half of water density
  float Scene::maxBodyDensity = 21.4f; // platinum density

  int Scene::numCircleVerticies = 64;

  size_t Scene::minIterations = 1;
  size_t Scene::maxIterations = 128;

  Scene::Scene()
    : registry(), grid(4, registry), gravity(0.0f, -9.81f), lightRenderer(nullptr), dateTime(nullptr), environment()
  {
    registry.on_construct<PhysicsComponent>().connect<&Scene::onPhysicsComponentCreate>(this);
    registry.on_update<PositionComponent>().connect<&Scene::onPositionComponentUpdate>();
  }

  void Scene::update(float elapsedTime, size_t iterations)
  {
    FLECTRON_PROFILE_EVENT("Scene::update");

    if (dateTime)
      dateTime->update(elapsedTime, environment);

    iterations = std::clamp(iterations, minIterations, maxIterations);
    float timeStep = elapsedTime / (float)iterations;

    // TODO check if this view could change between iterations
    auto view = registry.view<PhysicsComponent>();
    if (view.size() == 0)
      return;

    for (size_t i = 0; i < iterations; ++i)
    {
      // movement
      for (auto entity : view)
        view.get<PhysicsComponent>(entity).update(registry.get<PositionComponent>(entity), timeStep, gravity);

      // collisions
      Vector normal;
      float depth;
      for (auto entityA : view)
      {
        auto& pcA = registry.get<PositionComponent>(entityA);
        auto& vcA = registry.get<VertexComponent>(entityA);
        auto nearby = grid.getCells(vcA.getAABB(pcA));

        for (auto entityB : nearby)
        {
          if (entityA == entityB)
            continue;

          auto& phcA = view.get<PhysicsComponent>(entityA);
          auto& phcB = view.get<PhysicsComponent>(entityB);

          if (phcA.isStatic && phcB.isStatic)
            continue;

          auto& pcB = registry.get<PositionComponent>(entityB);
          auto& vcB = registry.get<VertexComponent>(entityB);
          if (collide(pcA, vcA, pcB, vcB, normal, depth))
          {
            if (phcA.isStatic)
            {
              pcB.move(normal * depth);
            }
            else if (phcB.isStatic)
            {
              pcA.move(-normal * depth);
            }
            else 
            {
              pcA.move(-normal * depth * 0.5f);
              pcB.move(normal * depth * 0.5f);
            }

            resolveCollision(phcA, phcB, normal);
            grid.insert(entityA);
            grid.insert(entityB);
          }
        }
      }
    }
  }

  void Scene::render(Window& window)
  {
    FLECTRON_PROFILE_EVENT("Scene::render");

    if (lightRenderer != nullptr)
      Renderer::offscreen();
    window.clear();

    auto renderables = registry.view<VertexComponent>();
    for (auto entity : renderables)
      if (registry.any_of<StrokeComponent, FillComponent, AnimationComponent, TextureComponent>(entity))
        window.draw({ entity, this });

    auto lights = registry.view<LightComponent>();
    if (!lights.empty())
    {
      if (lightRenderer == nullptr)
        throw std::runtime_error("LightRenderer not initialized");
      
      for (auto entity : lights)
        lightRenderer->addLight({ entity, this });
    }

    if (lightRenderer != nullptr)
      window.draw(lightRenderer, environment.nightColor, environment.darkness);
  }

  Entity Scene::createEntity(const std::string& name, const Vector& position, float rotation)
  {
    Entity entity(registry.create(), this);
    entity.add<TagComponent>(name);
    entity.add<PositionComponent>(&registry, position, rotation);
    return entity;
  }

  Entity Scene::createEntity()
  {
    return createEntity("Entity", { 0.0f, 0.0f }, 0.0f);
  }

  bool Scene::collide(PositionComponent& pcA, VertexComponent& vcA, PositionComponent& pcB, VertexComponent& vcB, Vector& normal, float& depth)
  {
    if (vcA.bodyType == BodyType::Box)
    {
      if (vcB.bodyType == BodyType::Box)
      {
        return intersectPolygons(pcA.position, vcA.getTransformedVertices(pcA), pcB.position, vcB.getTransformedVertices(pcB), normal, depth);
      }
      else if (vcB.bodyType == BodyType::Circle)
      {
        bool result = intersectCirclePolygon(pcB.position, vcB.radius, pcA.position, vcA.getTransformedVertices(pcA), normal, depth);
        if (result)
          normal = -normal;
        return result;
      }
    }
    else if (vcA.bodyType == BodyType::Circle)
    {
      if (vcB.bodyType == BodyType::Box)
      {
        return intersectCirclePolygon(pcA.position, vcA.radius, pcB.position, vcB.getTransformedVertices(pcB), normal, depth);
      }
      else if (vcB.bodyType == BodyType::Circle)
      {
        return intersectCircles(pcA.position, vcA.radius, pcB.position, vcB.radius, normal, depth);
      }
    }
    return false;
  }

  void Scene::resolveCollision(PhysicsComponent& phcA, PhysicsComponent& phcB, const Vector& normal)
  {
    Vector relativeVelocity = phcB.linearVelocity - phcA.linearVelocity;

    if (dot(relativeVelocity, normal) > 0.0f)
      return;

    float e = std::min(phcA.resitution, phcB.resitution);

    float j = -(1.0f + e) * dot(relativeVelocity, normal);

    j /= phcA.invMass + phcB.invMass;

    Vector impulse = j * normal;

    phcA.linearVelocity = phcA.linearVelocity - (impulse * phcA.invMass);
    phcB.linearVelocity = phcB.linearVelocity + (impulse * phcB.invMass);
  }

  void Scene::onPhysicsComponentCreate(entt::registry&, entt::entity entity)
  {
    grid.insert(entity);
  }

  void Scene::onPositionComponentUpdate(entt::registry& registry, entt::entity entity)
  {
    if (registry.all_of<VertexComponent>(entity))
    {
      auto& vc = registry.get<VertexComponent>(entity);
      vc.isTransformUpdateRequired = true;
      vc.isAABBUpdateRequired = true;
    }
    if (registry.all_of<TextureVertexComponent>(entity))
      registry.get<TextureVertexComponent>(entity).isTextureUpdateRequired = true;
  }

  void Scene::removeEntity(entt::entity entity)
  {
    grid.remove(entity);
    registry.destroy(entity);
  }

}