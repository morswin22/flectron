#pragma once
#include <entt/entt.hpp>
#include <flectron/assert/assert.hpp>

namespace flectron {

  class Entity
  { 
  private:
    entt::entity entityHandle;
    entt::registry* registry;
    
  public:
    Entity();
    Entity(entt::entity entityHandle, entt::registry* registry);

    template<typename Component>
    bool has() const
    {
      return registry->all_of<Component>(entityHandle);
    }

    template<typename... Components>
    bool hasAll() const
    {
      return registry->all_of<Components...>(entityHandle);
    }

    template<typename... Components>
    bool hasAny() const
    {
      return registry->any_of<Components...>(entityHandle);
    }

    template<typename Component, typename... Args>
    Component& add(Args&&... args)
    {
      FLECTRON_ASSERT(!has<Component>(), "Entity already has component");
      return registry->emplace<Component>(entityHandle, *this, std::forward<Args>(args)...);
    }

    template<typename Component>
    Component& get()
    {
      FLECTRON_ASSERT(has<Component>(), "Entity does not have component");
      return registry->get<Component>(entityHandle);
    }

    template<typename Component>
    void remove()
    {
      FLECTRON_ASSERT(has<Component>(), "Entity does not have component");
      registry->remove<Component>(entityHandle);
    }

    void destroy();

    template<typename Component>
    void patch()
    {
      FLECTRON_ASSERT(has<Component>(), "Entity does not have component");
      registry->patch<Component>(entityHandle);
    }

    operator bool() const;

    void render();

  };

}