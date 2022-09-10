#pragma once

#include <flectron/renderer/renderer.hpp>
#include <flectron/utils/memory.hpp>
#include <flectron/physics/vector.hpp>
#include <flectron/renderer/color.hpp>
#include <flectron/scene/components.hpp>

#ifndef FLECTRON_NUM_LIGHTS
#define FLECTRON_NUM_LIGHTS 512
#endif

namespace flectron
{

  class LightRenderer
  {
  private:
    Text vertexSource;
    Text fragmentSource;
    Shader::Pointer shader;

    float lightsData[FLECTRON_NUM_LIGHTS * 3];
    float lightsColors[FLECTRON_NUM_LIGHTS * 4];

    int currentLight;
    int currentData;
    int currentColor;

    GLuint vao;
    GLuint vbo;
    GLuint ibo;

  public:
    LightRenderer();
    ~LightRenderer();

    void reset();

    void render(const Color& baseColor, float darkness, const glm::vec3& cameraPosition, const glm::vec2& windowSize, GLuint rendererBuffer);

    void addLight(const Vector& position, float radius, const Color& color);
    void addLight(Entity entity);
  };

}
