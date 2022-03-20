#pragma once
#include <entt/entt.hpp>
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
      if (has<Component>())
        throw std::runtime_error("Entity already has component");
      return registry->emplace<Component>(entityHandle, *this, std::forward<Args>(args)...);
    }

    template<typename Component>
    Component& get()
    {
      if (!has<Component>())
        throw std::runtime_error("Entity does not have component");
      return registry->get<Component>(entityHandle);
    }

    template<typename Component>
    void remove()
    {
      if (!has<Component>())
        throw std::runtime_error("Entity does not have component");
      registry->remove<Component>(entityHandle);
    }

    void destroy();

    template<typename Component>
    void patch()
    {
      if (!has<Component>())
        throw std::runtime_error("Entity does not have component");
      registry->patch<Component>(entityHandle);
    }

    operator bool() const;

    void render();

  };

}