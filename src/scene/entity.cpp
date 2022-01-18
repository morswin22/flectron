#include <flectron/scene/entity.hpp>

namespace flectron {

  Entity::Entity()
    : entityHandle(entt::null), scene(nullptr) 
  {}

  Entity::Entity(entt::entity entityHandle, Scene* scene)
    : entityHandle(entityHandle), scene(scene) 
  {}

  void Entity::destroy()
  {
    scene->removeEntity(entityHandle);
  }

  Entity::operator bool() const {
    return entityHandle != entt::null;
  }

}