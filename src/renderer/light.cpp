#include <flectron/renderer/light.hpp>
#include <flectron/scene/entity.hpp>
#include <flectron/assert/assert.hpp>

namespace flectron
{

  LightRenderer::LightRenderer(const std::string& vertexPath, const std::string& fragmentPath)
    : shader(createRef<Shader>(vertexPath, fragmentPath)),
      currentLight(0), currentData(0), currentColor(0)
  {
    constexpr float positions[]{
      -1.0f, -1.0f,
      -1.0f,  1.0f,
       1.0f,  1.0f,
       1.0f, -1.0f
    };

    constexpr unsigned int indices[]{
      0, 1, 2,
      2, 3, 0
    };

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (const void*)0);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    shader->bind();
    shader->setUniform1i("uRendererTexture", 0);
  }

  LightRenderer::~LightRenderer()
  {
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);
    glDeleteVertexArrays(1, &vao);
  }

  void LightRenderer::reset()
  {
    currentLight = 0;
    currentData = 0;
    currentColor = 0;
  }

  void LightRenderer::render(const Color& baseColor, float darkness, const glm::vec3& cameraPosition, const glm::vec2& windowSize, GLuint rendererBuffer)
  {
    Renderer::endBatch();

    shader->bind();

    shader->setUniform2f("uCameraPosition", cameraPosition.x, cameraPosition.y);
    shader->setUniform2f("uWindowSize", windowSize.x, windowSize.y);
    shader->setUniform4f("uBaseColor", baseColor.r, baseColor.g, baseColor.b, baseColor.a);

    shader->setUniform4fv("uLightColor", lightsColors, currentLight);
    shader->setUniform3fv("uLightData", lightsData, currentLight);
    shader->setUniform1i("uLightCount", currentLight);
    shader->setUniform1f("uDarkness", darkness);

    Renderer::onscreen();
    glBindTextureUnit(0, rendererBuffer);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    reset();

    Renderer::beginBatch();
  }

  void LightRenderer::addLight(const Vector& position, float radius, const Color& color)
  {
    FLECTRON_ASSERT(currentLight < FLECTRON_NUM_LIGHTS, "Too many lights");

    lightsData[currentData++] = position.x;
    lightsData[currentData++] = position.y;
    lightsData[currentData++] = radius;

    lightsColors[currentColor++] = color.r;
    lightsColors[currentColor++] = color.g;
    lightsColors[currentColor++] = color.b;
    lightsColors[currentColor++] = color.a;

    currentLight += 1;
  }

  void LightRenderer::addLight(Entity entity)
  {
    FLECTRON_ASSERT(currentLight < FLECTRON_NUM_LIGHTS, "Too many lights");

    auto& pc = entity.get<PositionComponent>();
    auto& lc = entity.get<LightComponent>();

    lightsData[currentData++] = pc.position.x;
    lightsData[currentData++] = pc.position.y;
    lightsData[currentData++] = lc.lightRadius;

    lightsColors[currentColor++] = lc.lightColor.r;
    lightsColors[currentColor++] = lc.lightColor.g;
    lightsColors[currentColor++] = lc.lightColor.b;
    lightsColors[currentColor++] = lc.lightColor.a;
    
    currentLight += 1;
  }

}
