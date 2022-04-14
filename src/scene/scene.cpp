#include <flectron/scene/scene.hpp>
#include <flectron/scene/components.hpp>
#include <flectron/physics/math.hpp>
#include <flectron/physics/collisions.hpp>
#include <flectron/utils/profile.hpp>
#include <flectron/application/application.hpp>

namespace flectron 
{

  Environment::Environment()
    : gravity(0.0f, -9.81f), nightColor(Colors::darkGray()), darkness(0.0f)
  {}

  Environment::Environment(const Vector& gravity, const Color& nightColor, float darkness)
    : gravity(gravity), nightColor(nightColor), darkness(darkness)
  {}

  float Scene::minBodySize = 0.01f * 0.01f;
  float Scene::maxBodySize = 64.0f * 64.0f;

  float Scene::minBodyDensity = 0.5f; // half of water density
  float Scene::maxBodyDensity = 21.4f; // platinum density

  int Scene::numCircleVerticies = 24; // TODO make this be appropriate for the body size

  size_t Scene::minIterations = 1;
  size_t Scene::maxIterations = 128;

  Scene::Scene(size_t physicsIterations, size_t gridSize)
    : registry(), grid(static_cast<int>(gridSize), registry), environment(), lightRenderer(nullptr), dateTime(nullptr), physicsIterations(physicsIterations)
  {
    FLECTRON_LOG_TRACE("Creating scene");
    registry.on_construct<PhysicsComponent>().connect<&Scene::onPhysicsComponentCreate>(this);
    registry.on_destroy<SpatialHashGridComponent>().connect<&Scene::onSpatialHashGridComponentDestroy>(this);
    registry.on_update<PositionComponent>().connect<&Scene::onPositionComponentUpdate>();
    registry.on_construct<PolygonComponent>().connect<&Scene::onBodyDefiningComponentCreate>();
    registry.on_construct<BoxComponent>().connect<&Scene::onBodyDefiningComponentCreate>();
    registry.on_construct<CircleComponent>().connect<&Scene::onBodyDefiningComponentCreate>();
  }

  Scene::~Scene()
  {
    FLECTRON_LOG_TRACE("Destroying scene");
  }

  void Scene::update(Application& application)
  {
    FLECTRON_PROFILE_FRAME("Scene::update");

    auto scriptComponents = getScriptComponents();
    auto scriptsEnd = scriptComponents.end();
    auto scriptsIterator = updateScriptComponents(FLECTRON_PHYSICS, scriptComponents.begin(), scriptsEnd);

    if (dateTime)
      dateTime->update(application.elapsedTime, environment);

    for (auto& entity : registry.view<AnimationComponent>())
      registry.get<AnimationComponent>(entity).update(application.elapsedTime);

    updatePhysics(application.elapsedTime, physicsIterations);
    
    scriptsIterator = updateScriptComponents(FLECTRON_RENDER, scriptsIterator, scriptsEnd);

    render(application.window);
    updateScriptComponents(std::numeric_limits<int>::max(), scriptsIterator, scriptsEnd);
  }

  void Scene::updatePhysics(float elapsedTime, size_t iterations)
  {
    FLECTRON_PROFILE_EVENT("Scene::updatePhysics");
    iterations = std::clamp(iterations, minIterations, maxIterations);
    float timeStep = elapsedTime / (float)iterations;

    auto view = registry.view<PhysicsComponent>();
    if (view.size() == 0)
      return;

    for (size_t i = 0; i < iterations; ++i)
    {
      // movement
      for (auto entity : view)
        view.get<PhysicsComponent>(entity).update(registry.get<PositionComponent>(entity), timeStep, environment.gravity);

      // collisions
      Collision collision;
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
          if (collide(pcA, vcA, pcB, vcB, collision))
          {
            if (phcA.isStatic)
            {
              pcB.move(collision.normal * collision.depth);
            }
            else if (phcB.isStatic)
            {
              pcA.move(-collision.normal * collision.depth);
            }
            else 
            {
              pcA.move(-collision.normal * collision.depth * 0.5f);
              pcB.move( collision.normal * collision.depth * 0.5f);
            }

            resolveCollision(phcA, phcB, collision);
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
        Entity(entity, &registry).render();

    auto lights = registry.view<LightComponent>();
    if (!lights.empty())
    {
      FLECTRON_ASSERT(lightRenderer != nullptr, "LightRenderer not initialized");
      
      for (auto entity : lights)
        lightRenderer->addLight({ entity, &registry });
    }

    if (lightRenderer != nullptr)
    {
      const float scale = window.camera.getScale();
      lightRenderer->render(
        environment.nightColor,
        environment.darkness,
        window.camera.getPosition(),
        { window.properties.width * scale, window.properties.height * scale },
        window.rendererBuffer);
    }
  }

  Entity Scene::createEntity(const std::string& name, const Vector& position, float rotation)
  {
    Entity entity(registry.create(), &registry);
    entity.add<TagComponent>(name);
    entity.add<PositionComponent>(position, rotation);
    return entity;
  }

  Entity Scene::createEntity()
  {
    return createEntity("Entity", { 0.0f, 0.0f }, 0.0f);
  }

  Entity Scene::createScript(const std::string& name, std::function<void()> callback, int order)
  {
    Entity entity(registry.create(), &registry);
    entity.add<TagComponent>(name);
    entity.add<ScriptComponent>(callback, order);
    return entity;
  }

  ScriptComponentIterator Scene::updateScriptComponents(int max, ScriptComponentIterator iterator, ScriptComponentIterator end)
  {
    for (; iterator != end; ++iterator)
    {
      auto& script = registry.get<ScriptComponent>(*iterator);
      if (script.order >= max)
        break;
      script.callback();
    }
    return iterator;
  }

  void Scene::onPhysicsComponentCreate(entt::registry&, entt::entity entity)
  {
    grid.insert(entity);
  }

  void Scene::onSpatialHashGridComponentDestroy(entt::registry&, entt::entity entity)
  {
    grid.remove(entity);
  }

  void Scene::onPositionComponentUpdate(entt::registry& registry, entt::entity entity)
  {
    if (registry.all_of<VertexComponent>(entity))
    {
      auto& vc = registry.get<VertexComponent>(entity);
      vc.isTransformUpdateRequired = true;
      vc.isTransformCenterUpdateRequired = true;
      vc.isAABBUpdateRequired = true;
    }
    if (registry.all_of<TextureVertexComponent>(entity))
      registry.get<TextureVertexComponent>(entity).isTextureUpdateRequired = true;
  }

  void Scene::onBodyDefiningComponentCreate(entt::registry& registry, entt::entity entity)
  {
    registry.emplace_or_replace<VertexComponent>(entity, Entity(entity, &registry));
  }

  void Scene::clear()
  {
    registry.clear();
    grid.clear();

    if (lightRenderer != nullptr)
      lightRenderer->reset();

    if (dateTime != nullptr)
      dateTime->reset();
  }

  void Scene::removeEntity(entt::entity entity)
  {
    registry.destroy(entity);
  }

}