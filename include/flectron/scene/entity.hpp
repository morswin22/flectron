#pragma once
#include <flectron/scene/scene.hpp>

namespace flectron {

  class Entity
  { 
  private:
    entt::entity entityHandle;
    Scene* scene; // TODO maybe use std::weak_ptr
    
  public:
    Entity();
    Entity(entt::entity entityHandle, Scene* scene);

    template<typename Component>
    bool has() const
    {
      return scene->registry.all_of<Component>(entityHandle);
    }

    template<typename... Components>
    bool hasAll() const
    {
      return scene->registry.all_of<Components>(entityHandle);
    }

    template<typename... Components>
    bool hasAny() const
    {
      return scene->registry.any_of<Components>(entityHandle);
    }

    template<typename Component, typename... Args>
    Component& add(Args&&... args)
    {
      if (has<Component>())
        throw std::runtime_error("Entity already has component");
      return scene->registry.emplace<Component>(entityHandle, std::forward<Args>(args)...);
    }

    template<typename Component>
    Component& get()
    {
      if (!has<Component>())
        throw std::runtime_error("Entity does not have component");
      return scene->registry.get<Component>(entityHandle);
    }

    template<typename Component>
    void remove()
    {
      if (!has<Component>())
        throw std::runtime_error("Entity does not have component");
      scene->registry.remove<Component>(entityHandle);
    }

    void destroy();

    template<typename Component>
    void patch()
    {
      if (!has<Component>())
        throw std::runtime_error("Entity does not have component");
      scene->registry.patch<Component>(entityHandle);
    }

    operator bool() const;

  };

}