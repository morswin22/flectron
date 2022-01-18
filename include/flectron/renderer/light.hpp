#pragma once

#include <flectron/renderer/renderer.hpp>
#include <flectron/utils/memory.hpp>
#include <flectron/physics/vector.hpp>
#include <flectron/renderer/color.hpp>
#include <flectron/scene/components.hpp>

namespace flectron
{

  // TODO maybe convert into a singleton
  class LightRenderer
  {
  private:
    Ref<Shader> shader;
    float lightsData[128 * 3];
    float lightsColors[128 * 4];

    int currentLight;
    int currentData;
    int currentColor;

    GLuint vao;
    GLuint vbo;
    GLuint ibo;

  public:
    LightRenderer(const std::string& vertexPath, const std::string& fragmentPath);
    ~LightRenderer();

    void reset();

    void render(const Color& baseColor, float darkness, const glm::vec3& cameraPosition, const glm::vec2& windowSize, GLuint rendererBuffer);

    void addLight(const Vector& position, float radius, const Color& color);
    void addLight(Entity entity);
  };

}
