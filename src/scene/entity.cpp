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

  Entity::operator bool() const {
    return entityHandle != entt::null;
  }

  void Entity::render()
  {
    auto& pc = get<PositionComponent>();
    auto& vc = get<VertexComponent>();

    if (has<StrokeComponent>())
    {
      // TODO improve stroke rendering
      auto& vertices = vc.getTransformedVertices(pc);
      auto& sc = get<StrokeComponent>();
      for (int j = 0; j < vertices.size(); j++)
        Renderer::debugLine(vertices[j], vertices[(j + 1) % vertices.size()], sc.strokeColor);
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
        Renderer::quad(vertices[0], vertices[1], vertices[2], vertices[3], static_cast<Texture*>(ac.animationAtlas.get())->get(), *frame, color);
      }
      else if (has<BoxComponent>())
      {
        auto& vertices = vc.getTransformedVertices(pc);
        Renderer::quad(vertices[0], vertices[1], vertices[2], vertices[3], static_cast<Texture*>(ac.animationAtlas.get())->get(), *frame, color);
      }
      else
      {
        throw std::runtime_error("Use TextureVertexComponent for non-box bodies");
      }
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
      {
        throw std::runtime_error("Use TextureVertexComponent for non-box bodies");
      }
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