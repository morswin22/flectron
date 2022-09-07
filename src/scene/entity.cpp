#include <flectron/scene/entity.hpp>
#include <flectron/scene/components.hpp>
#include <flectron/renderer/renderer.hpp>

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
    entityHandle = entt::null;
  }

  Entity::operator bool() const 
  {
    return entityHandle != entt::null;
  }

  bool Entity::operator==(const Entity& other) const
  {
    return entityHandle == other.entityHandle && registry == other.registry;
  }

  bool Entity::operator!=(const Entity& other) const
  {
    return entityHandle != other.entityHandle || registry != other.registry;
  }

  bool Entity::operator==(const entt::entity& other) const
  {
    return entityHandle == other;
  }

  bool Entity::operator!=(const entt::entity& other) const
  {
    return entityHandle != other;
  }

  void Entity::render()
  {
    auto& pc = get<PositionComponent>();
    auto& vc = get<VertexComponent>();

    if (has<StrokeComponent>())
    {
      auto& sc = get<StrokeComponent>();
      if (has<CircleComponent>())
      {
        auto& cc = get<CircleComponent>();
        Renderer::circle(pc.position, cc.radius + sc.strokeWidth, cc.thickness, cc.fade, sc.strokeColor); // TODO what about the thickness and fade?
      }
      else
      {
        auto& vertices = vc.getTransformedVertices(pc);
        for (int j = 0; j < vertices.size(); j++)
          Renderer::line(vertices[j], vertices[(j + 1) % vertices.size()], sc.strokeWidth, sc.strokeColor);
      }
    }

    if (has<AnimationComponent>())
    {
      auto& ac = get<AnimationComponent>();
      glm::vec4* frame = ac.animationAtlas->getAnimation(ac.animationState.currentName)->getNext(ac.animationState);
      Color color = has<FillComponent>() ? get<FillComponent>().fillColor : Colors::white();
      if (has<TextureVertexComponent>())
      {
        auto& tvc = get<TextureVertexComponent>();
        auto& vertices = tvc.getTransformedVertices(pc);
        Renderer::quad(vertices[0], vertices[1], vertices[2], vertices[3], ac.animationAtlas->image->getGPU(), *frame, color);
      }
      else if (has<BoxComponent>())
      {
        auto& vertices = vc.getTransformedVertices(pc);
        Renderer::quad(vertices[0], vertices[1], vertices[2], vertices[3], ac.animationAtlas->image->getGPU(), *frame, color);
      }
      else
        FLECTRON_ASSERT(false, "Use TextureVertexComponent for non-box bodies");
    }
    else if (has<TextureComponent>())
    {
      auto& tc = get<TextureComponent>();
      Color color = has<FillComponent>() ? get<FillComponent>().fillColor : Colors::white();
      if (has<TextureVertexComponent>())
      {
        auto& tvc = get<TextureVertexComponent>();
        auto& vertices = tvc.getTransformedVertices(pc);
        Renderer::quad(vertices[0], vertices[1], vertices[2], vertices[3], tc.textureIndex, tc.texturePositions, color);
      }
      else if (has<BoxComponent>())
      {
        auto& vertices = vc.getTransformedVertices(pc);
        Renderer::quad(vertices[0], vertices[1], vertices[2], vertices[3], tc.textureIndex, tc.texturePositions, color);
      }
      else
        FLECTRON_ASSERT(false, "Use TextureVertexComponent for non-box bodies");
    }
    else if (has<FillComponent>())
    {
      if (has<CircleComponent>())
      {
        auto& cc = get<CircleComponent>();
        Renderer::circle(pc.position, cc.radius, cc.thickness, cc.fade, get<FillComponent>().fillColor);
      }
      else
      {
        Renderer::polygon(vc.getTransformedVertices(pc), vc.triangles, get<FillComponent>().fillColor);
      }
    }
  }

}