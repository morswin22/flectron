#pragma once
#include <entt/entt.hpp>
#include <flectron/scene/datetime.hpp>
#include <flectron/scene/components.hpp>
#include <flectron/scene/grid.hpp>
#include <flectron/scene/window.hpp>
#include <flectron/renderer/light.hpp>
#include <flectron/scene/entity.hpp>

namespace flectron
{

  struct Environment
  {
    Color nightColor;
    float darkness;

    Environment();
    Environment(const Color& nightColor, float darkness);
  };

  class Scene
  {
  public:
    static float minBodySize;
    static float maxBodySize;

    static float minBodyDensity; // g/cm^3
    static float maxBodyDensity;

    static int numCircleVerticies;

    static size_t minIterations;
    static size_t maxIterations;

  private:
    entt::registry registry;
    SpatialHashGrid grid;

  public:
    Vector gravity; // TODO put gravity in environment
    Environment environment;
    Scope<LightRenderer> lightRenderer;
    Scope<DateTime> dateTime;

  public:
    Scene();

    void update(float elapsedTime, size_t iterations);
    void render(Window& window);

    friend class Entity;
    Entity createEntity(const std::string& name, const Vector& position, float rotation);
    Entity createEntity();
    Entity createScript(const std::string& name, std::function<void()> callback, int order = 0);

    void sortScriptComponents();
    void updateScriptComponents(int min, int max);

    template<typename ...Components>
    size_t getEntityCount() const
    {
      return registry.view<Components...>().size();
    }

    size_t getEntityCount(const std::string& tag) const
    {
      size_t count = 0;
      auto view = registry.view<TagComponent>();
      for (auto entity : view)
        if (registry.get<TagComponent>(entity).tag == tag)
          ++count;
      return count;
    }

    void onPhysicsComponentCreate(entt::registry&, entt::entity entity);
    void onSpatialHashGridComponentDestroy(entt::registry&, entt::entity entity);
    static void onPositionComponentUpdate(entt::registry& registry, entt::entity entity);
    static void onBodyDefiningComponentCreate(entt::registry& registry, entt::entity entity);

    void removeEntity(entt::entity entity);

    template<typename ...Components>
    void removeEntitiesOutside(const Constraints& constraints)
    {
      auto view = registry.view<Components...>();
      for (auto entity : view)
      {
        auto& pc = registry.get<PositionComponent>(entity);
        if (registry.all_of<VertexComponent>(entity))
        {
          const AABB& box = registry.get<VertexComponent>(entity).getAABB(pc);

          if (box.max.x < constraints.left || box.min.x > constraints.right ||
              box.max.y < constraints.top  || box.min.y > constraints.bottom)
          {
            removeEntity(entity);
          }
        }
        else
        {
          if (pc.position.x < constraints.left || pc.position.x > constraints.right ||
              pc.position.y < constraints.top  || pc.position.y > constraints.bottom)
          {
            removeEntity(entity);
          }
        }
      }
    }
  };

}