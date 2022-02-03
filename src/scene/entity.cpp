#include <flectron/scene/entity.hpp>

namespace flectron {

  Entity::Entity()
    : entityHandle(entt::null), registry(nullptr) 
  {}

  Entity::Entity(entt::entity entityHandle, entt::registry* registry)
    : entityHandle(entityHandle), registry(registry) 
  {}

  void Entity::destroy()
  {
    registry->destroy(entityHandle);
  }

  Entity::operator bool() const {
    return entityHandle != entt::null;
  }

}