#include <flectron/renderer/renderer.hpp>

#include <exception>
#include <fstream>
#include <sstream>
#include <iostream>
#include <array>

#include <flectron/renderer/color.hpp>
#include <flectron/physics/vector.hpp>
#include <flectron/physics/math.hpp>
#include <flectron/physics/transform.hpp>
#include <flectron/physics/aabb.hpp>
#include <flectron/utils/random.hpp>
#include <flectron/utils/profile.hpp>
#include <flectron/assert/log.hpp>
#include <stb_image.h>
#include <glm/gtc/type_ptr.hpp>
#include <flectron/assets/text.hpp>

FLECTRON_EMBED(FLECTRON_SHADER_TEXTURE_VERT);
FLECTRON_EMBED(FLECTRON_SHADER_TEXTURE_FRAG);
FLECTRON_EMBED(FLECTRON_SHADER_CIRCLE_VERT);
FLECTRON_EMBED(FLECTRON_SHADER_CIRCLE_FRAG);
FLECTRON_EMBED(FLECTRON_SHADER_LINE_VERT);
FLECTRON_EMBED(FLECTRON_SHADER_LINE_GEOM);
FLECTRON_EMBED(FLECTRON_SHADER_LINE_FRAG);

namespace flectron
{

  static const std::size_t MaxTriangleCount = 10000;
  static const std::size_t MaxVertexCount = MaxTriangleCount * 3;
  static const std::size_t MaxIndexCount = MaxTriangleCount * 3;
  static size_t MaxTextureSlots;

  struct TextureVertex
  {
    glm::vec2 position;
    glm::vec4 color;
    glm::vec2 textureCoord;
    float textureIndex;
    float tilingFactor;
  };

  struct CircleVertex
  {
    glm::vec2 worldPosition;
    glm::vec2 localPosition;
    glm::vec4 color;
    float thickness;
    float fade;
  };

  struct LineVertex
  {
    glm::vec2 position;
    glm::vec4 color;
    float thickness;
  };

  struct RendererData
  {
    GLuint frameBuffer = 0;

    // Texture rendering
    GLuint textureVertexArray = 0;
    GLuint textureVertexBuffer = 0;
    GLuint textureIndexBuffer = 0;

    TextureVertex* textureBuffer = nullptr;
    TextureVertex* textureBufferPointer = nullptr;

    uint32_t* textureIndices = nullptr;
    uint32_t* textureIndicesPointer = nullptr;

    uint32_t textureIndexCount = 0;
    uint32_t textureOffset = 0;

    GLuint whiteTexture = 0;

    uint32_t* textureSlots = nullptr;
    uint32_t textureSlotIndex = 1;

    Text textureShaderVertex;
    Text textureShaderFragment;
    Shader::Ref textureShader = nullptr;

    // Circle rendering
    GLuint circleVertexArray = 0;
    GLuint circleVertexBuffer = 0;
    GLuint circleIndexBuffer = 0;
    
    CircleVertex* circleBuffer = nullptr;
    CircleVertex* circleBufferPointer = nullptr;

    uint32_t circleIndexCount = 0;

    glm::vec2 circleVertexPositions[4];

    Text circleShaderVertex;
    Text circleShaderFragment;
    Shader::Ref circleShader = nullptr;

    // Line rendering
    GLuint lineVertexArray = 0;
    GLuint lineVertexBuffer = 0;

    LineVertex* lineBuffer = nullptr;
    LineVertex* lineBufferPointer = nullptr;

    uint32_t lineIndexCount = 0;
    
    Text lineShaderVertex;
    Text lineShaderFragment;
    Text lineShaderGeometry;
    Shader::Ref lineShader = nullptr;

    // Statistics
    Renderer::Statistics statistics;

    // Camera
    GLuint cameraUniformBuffer = 0;
  };

  static RendererData rendererData;

  void Renderer::initTextureRendering()
  {
    rendererData.textureBuffer = new TextureVertex[MaxVertexCount];

    glCreateVertexArrays(1, &rendererData.textureVertexArray);
    glBindVertexArray(rendererData.textureVertexArray);

    glCreateBuffers(1, &rendererData.textureVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, rendererData.textureVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, MaxVertexCount * sizeof(TextureVertex), nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexArrayAttrib(rendererData.textureVertexArray, 0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(TextureVertex), (const void*)offsetof(TextureVertex, position));

    glEnableVertexArrayAttrib(rendererData.textureVertexArray, 1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(TextureVertex), (const void*)offsetof(TextureVertex, color));

    glEnableVertexArrayAttrib(rendererData.textureVertexArray, 2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(TextureVertex), (const void*)offsetof(TextureVertex, textureCoord));

    glEnableVertexArrayAttrib(rendererData.textureVertexArray, 3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(TextureVertex), (const void*)offsetof(TextureVertex, textureIndex));

    glEnableVertexArrayAttrib(rendererData.textureVertexArray, 4);
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(TextureVertex), (const void*)offsetof(TextureVertex, tilingFactor));

    rendererData.textureIndices = new uint32_t[MaxIndexCount];

    glGenBuffers(1, &rendererData.textureIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererData.textureIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, MaxIndexCount * sizeof(uint32_t), nullptr, GL_DYNAMIC_DRAW);

    GLint tempMaxTextureSlots;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &tempMaxTextureSlots);
    MaxTextureSlots = (size_t)tempMaxTextureSlots;
    rendererData.textureSlots = new uint32_t[MaxTextureSlots];

    glCreateTextures(GL_TEXTURE_2D, 1, &rendererData.whiteTexture);
    glBindTexture(GL_TEXTURE_2D, rendererData.whiteTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    uint32_t white = 0xffffffff;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &white);

    rendererData.textureSlots[0] = rendererData.whiteTexture;
    for (size_t i = 1; i < MaxTextureSlots; i++)
      rendererData.textureSlots[i] = 0;

    int* samplers = new int[MaxTextureSlots];
    for (size_t i = 0; i < MaxTextureSlots; i++)
      samplers[i] = (int)i;

    rendererData.textureShaderVertex = Text::fromEmbed(FLECTRON_SHADER_TEXTURE_VERT());
    rendererData.textureShaderVertex.load();

    rendererData.textureShaderFragment = Text::fromEmbed(FLECTRON_SHADER_TEXTURE_FRAG());
    rendererData.textureShaderFragment.load();

    // rendererData.textureShader = Shader::create({
    //   .vertex = rendererData.textureShaderVertex,
    //   .fragment = rendererData.textureShaderFragment
    // });
    rendererData.textureShader = Shader::create({
      rendererData.textureShaderVertex,
      nullptr,
      rendererData.textureShaderFragment,
      nullptr
    });
    rendererData.textureShader->bind();
    rendererData.textureShader->setUniform1iv("uTextures", samplers, tempMaxTextureSlots);
    rendererData.textureShader->setUniform1f("uZIndex", 0.3f);
    
    delete[] samplers;
  }

  void Renderer::initCircleRendering()
  {
    rendererData.circleBuffer = new CircleVertex[MaxVertexCount];

    glCreateVertexArrays(1, &rendererData.circleVertexArray);
    glBindVertexArray(rendererData.circleVertexArray);

    glCreateBuffers(1, &rendererData.circleVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, rendererData.circleVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, MaxVertexCount * sizeof(CircleVertex), nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexArrayAttrib(rendererData.circleVertexArray, 0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(CircleVertex), (const void*)offsetof(CircleVertex, worldPosition));

    glEnableVertexArrayAttrib(rendererData.circleVertexArray, 1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(CircleVertex), (const void*)offsetof(CircleVertex, localPosition));

    glEnableVertexArrayAttrib(rendererData.circleVertexArray, 2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(CircleVertex), (const void*)offsetof(CircleVertex, color));

    glEnableVertexArrayAttrib(rendererData.circleVertexArray, 3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(CircleVertex), (const void*)offsetof(CircleVertex, fade));

    glEnableVertexArrayAttrib(rendererData.circleVertexArray, 4);
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(CircleVertex), (const void*)offsetof(CircleVertex, thickness));

    uint32_t* indices = new uint32_t[MaxIndexCount];
    uint32_t offset = 0;

    for (uint32_t i = 0; i < MaxIndexCount; i += 6)
    {
      indices[i + 0] = offset + 0;
      indices[i + 1] = offset + 1;
      indices[i + 2] = offset + 2;

      indices[i + 3] = offset + 2;
      indices[i + 4] = offset + 3;
      indices[i + 5] = offset + 0;

      offset += 4;
    }

    glCreateBuffers(1, &rendererData.circleIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererData.circleIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, MaxIndexCount * sizeof(uint32_t), indices, GL_STATIC_DRAW);

    delete[] indices;

    rendererData.circleVertexPositions[0] = glm::vec2(-0.5f, -0.5f) * 2.0f;
		rendererData.circleVertexPositions[1] = glm::vec2( 0.5f, -0.5f) * 2.0f;
		rendererData.circleVertexPositions[2] = glm::vec2( 0.5f,  0.5f) * 2.0f;
		rendererData.circleVertexPositions[3] = glm::vec2(-0.5f,  0.5f) * 2.0f;

    rendererData.circleShaderVertex = Text::fromEmbed(FLECTRON_SHADER_CIRCLE_VERT());
    rendererData.circleShaderVertex.load();

    rendererData.circleShaderFragment = Text::fromEmbed(FLECTRON_SHADER_CIRCLE_FRAG());
    rendererData.circleShaderFragment.load();
    
    rendererData.circleShader = Shader::create({
      rendererData.circleShaderVertex,
      nullptr,
      rendererData.circleShaderFragment,
      nullptr
    });
    rendererData.circleShader->bind();
    rendererData.circleShader->setUniform1f("uZIndex", 0.2f);
  }

  void Renderer::initLineRendering()
  {
    rendererData.lineBuffer = new LineVertex[MaxVertexCount];

    glCreateVertexArrays(1, &rendererData.lineVertexArray);
    glBindVertexArray(rendererData.lineVertexArray);

    glCreateBuffers(1, &rendererData.lineVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, rendererData.lineVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, MaxVertexCount * sizeof(LineVertex), nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexArrayAttrib(rendererData.lineVertexArray, 0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (const void*)offsetof(LineVertex, position));

    glEnableVertexArrayAttrib(rendererData.lineVertexArray, 1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (const void*)offsetof(LineVertex, color));

    glEnableVertexArrayAttrib(rendererData.lineVertexArray, 2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (const void*)offsetof(LineVertex, thickness));

    rendererData.lineShaderVertex = Text::fromEmbed(FLECTRON_SHADER_LINE_VERT());
    rendererData.lineShaderVertex.load();

    rendererData.lineShaderFragment = Text::fromEmbed(FLECTRON_SHADER_LINE_FRAG());
    rendererData.lineShaderFragment.load();

    rendererData.lineShaderGeometry = Text::fromEmbed(FLECTRON_SHADER_LINE_GEOM());
    rendererData.lineShaderGeometry.load();

    rendererData.lineShader = Shader::create({
      rendererData.lineShaderVertex,
      rendererData.lineShaderGeometry,
      rendererData.lineShaderFragment,
      nullptr
    });
    rendererData.lineShader->bind();
    rendererData.lineShader->setUniform1f("uZIndex", 0.1f);
  }

  void Renderer::initCamera()
  {
    glGenBuffers(1, &rendererData.cameraUniformBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, rendererData.cameraUniformBuffer);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
  }

  void Renderer::init(int width, int height, GLuint& buffer)
  {
    FLECTRON_LOG_TRACE("Initializing renderer");
    initTextureRendering();
    initCircleRendering();
    initLineRendering();
    rendererData.statistics.reset();
    rendererData.frameBuffer = createFrameBuffer(width, height, buffer);
    initCamera();
  }

  void Renderer::shutdown()
  {
    FLECTRON_LOG_TRACE("Shutting down renderer");

    glDeleteVertexArrays(1, &rendererData.textureVertexArray);
    glDeleteBuffers(1, &rendererData.textureVertexBuffer);
    glDeleteBuffers(1, &rendererData.textureIndexBuffer);
    glDeleteTextures(1, &rendererData.whiteTexture);

    glDeleteVertexArrays(1, &rendererData.circleVertexArray);
    glDeleteBuffers(1, &rendererData.circleVertexBuffer);
    glDeleteBuffers(1, &rendererData.circleIndexBuffer);

    glDeleteVertexArrays(1, &rendererData.lineVertexArray);
    glDeleteBuffers(1, &rendererData.lineVertexBuffer);

    if (rendererData.frameBuffer != 0)
      glDeleteFramebuffers(1, &rendererData.frameBuffer);

    glDeleteBuffers(1, &rendererData.cameraUniformBuffer);

    delete[] rendererData.textureBuffer;
    delete[] rendererData.textureIndices;
    delete[] rendererData.textureSlots;

    delete[] rendererData.circleBuffer;

    delete[] rendererData.lineBuffer;

    rendererData.textureShaderVertex.unload();
    rendererData.textureShaderFragment.unload();
    
    rendererData.circleShaderVertex.unload();
    rendererData.circleShaderFragment.unload();

    rendererData.lineShaderVertex.unload();
    rendererData.lineShaderFragment.unload();
  }

  void Renderer::setViewProjectionMatrix(const Camera& camera)
  {
    glBindBuffer(GL_UNIFORM_BUFFER, rendererData.cameraUniformBuffer);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), glm::value_ptr(camera.getViewProjectionMatrix()), GL_DYNAMIC_DRAW);
  }

  void Renderer::beginBatch()
  {
    beginTextureBatch();
    beginCircleBatch();
    beginLineBatch();
  }

  void Renderer::endBatch()
  {
    endTextureBatch();
    endCircleBatch();
    endLineBatch();
  }

  void Renderer::beginTextureBatch()
  {
    rendererData.textureBufferPointer = rendererData.textureBuffer;
    rendererData.textureIndicesPointer = rendererData.textureIndices;
    rendererData.textureIndexCount = 0;
    rendererData.textureOffset = 0;
    rendererData.textureSlotIndex = 1;
  }

  void Renderer::endTextureBatch()
  {
    if (rendererData.textureIndexCount == 0)
      return;

    rendererData.textureShader->bind();
    rendererData.textureShader->setUniformBlock("CameraBlock", rendererData.cameraUniformBuffer);

    glBindVertexArray(rendererData.textureVertexArray);

    GLsizeiptr textureSize = (uint8_t*)rendererData.textureBufferPointer - (uint8_t*)rendererData.textureBuffer;
    glBindBuffer(GL_ARRAY_BUFFER, rendererData.textureVertexBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, textureSize, rendererData.textureBuffer);

    GLsizeiptr indexSize = (uint8_t*)rendererData.textureIndicesPointer - (uint8_t*)rendererData.textureIndices;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererData.textureIndexBuffer);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indexSize, rendererData.textureIndices);

    for (uint32_t i = 0; i < rendererData.textureSlotIndex; i++)
      glBindTextureUnit(i, rendererData.textureSlots[i]);

    glDrawElements(GL_TRIANGLES, rendererData.textureIndexCount, GL_UNSIGNED_INT, nullptr);
  }

  void Renderer::beginCircleBatch()
  {
    rendererData.circleBufferPointer = rendererData.circleBuffer;
    rendererData.circleIndexCount = 0;
  }

  void Renderer::endCircleBatch()
  {
    if (rendererData.circleIndexCount == 0)
      return;

    rendererData.circleShader->bind();
    rendererData.circleShader->setUniformBlock("CameraBlock", rendererData.cameraUniformBuffer);

    glBindVertexArray(rendererData.circleVertexArray);

    GLsizeiptr circleSize = (uint8_t*)rendererData.circleBufferPointer - (uint8_t*)rendererData.circleBuffer;
    glBindBuffer(GL_ARRAY_BUFFER, rendererData.circleVertexBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, circleSize, rendererData.circleBuffer);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererData.circleIndexBuffer);

    glDrawElements(GL_TRIANGLES, rendererData.circleIndexCount, GL_UNSIGNED_INT, nullptr);
  }

  void Renderer::beginLineBatch()
  {
    rendererData.lineBufferPointer = rendererData.lineBuffer;
    rendererData.lineIndexCount = 0;
  }

  void Renderer::endLineBatch()
  {
    if (rendererData.lineIndexCount == 0)
      return;

    rendererData.lineShader->bind();
    rendererData.lineShader->setUniformBlock("CameraBlock", rendererData.cameraUniformBuffer);

    glBindVertexArray(rendererData.lineVertexArray);

    GLsizeiptr lineSize = (uint8_t*)rendererData.lineBufferPointer - (uint8_t*)rendererData.lineBuffer;
    glBindBuffer(GL_ARRAY_BUFFER, rendererData.lineVertexBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, lineSize, rendererData.lineBuffer);

    glDrawArrays(GL_LINES, 0, rendererData.lineIndexCount);
  }

  void Renderer::onscreen()
  {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  void Renderer::offscreen()
  {
    glBindFramebuffer(GL_FRAMEBUFFER, rendererData.frameBuffer);
  }

  void Renderer::square(const Vector& position, float size, const Color& color)
  {
    square(position, size, rendererData.whiteTexture, 1.0f, { 0.0f, 0.0f, 1.0f, 1.0f }, color);
  }

  void Renderer::square(const Vector& position, float size, uint32_t textureID, float tilingFactor, const Color& tint)
  {
    square(position, size, textureID, tilingFactor, { 0.0f, 0.0f, 1.0f, 1.0f }, tint);
  }

  void Renderer::square(const Vector& position, float size, uint32_t textureID, const glm::vec4& texturePosition, const Color& tint)
  {
    square(position, size, textureID, 1.0f, texturePosition, tint);
  }

  void Renderer::square(const Vector& position, float size, uint32_t textureID, float tilingFactor, const glm::vec4& texturePosition, const Color& tint)
  {
    // TODO for now assume that position is the left most corner
    quad(
      { position.x, position.y },
      { position.x + size, position.y },
      { position.x + size, position.y + size },
      { position.x, position.y + size },
      textureID, tilingFactor, texturePosition, tint);
  }

  void Renderer::square(const Vector& position, float size, float rotation, const Color& color)
  {
    square(position, size, rotation, rendererData.whiteTexture, 1.0f, { 0.0f, 0.0f, 1.0f, 1.0f }, color);
  }

  void Renderer::square(const Vector& position, float size, float rotation, uint32_t textureID, float tilingFactor, const Color& tint)
  {
    square(position, size, rotation, textureID, tilingFactor, { 0.0f, 0.0f, 1.0f, 1.0f }, tint);
  }

  void Renderer::square(const Vector& position, float size, float rotation, uint32_t textureID, const glm::vec4& texturePosition, const Color& tint)
  {
    square(position, size, rotation, textureID, 1.0f, texturePosition, tint);
  }

  void Renderer::square(const Vector& position, float size, float rotation, uint32_t textureID, float tilingFactor, const glm::vec4& texturePosition, const Color& tint)
  {
    // TODO for now assume that the rotation is around the corner of the square
    Transform tf(position, rotation);
    quad(
      transform({ position.x, position.y }, tf),
      transform({ position.x + size, position.y }, tf),
      transform({ position.x + size, position.y + size }, tf),
      transform({ position.x, position.y + size }, tf),
      textureID, tilingFactor, texturePosition, tint);
  }

  void Renderer::rect(const Vector& position, const Vector& size, const Color& color)
  {
    rect(position, size, rendererData.whiteTexture, 1.0f, { 0.0f, 0.0f, 1.0f, 1.0f }, color);
  }

  void Renderer::rect(const Vector& position, const Vector& size, uint32_t textureID, float tilingFactor, const Color& tint)
  {
    rect(position, size, textureID, tilingFactor, { 0.0f, 0.0f, 1.0f, 1.0f }, tint);
  }
  void Renderer::rect(const Vector& position, const Vector& size, uint32_t textureID, const glm::vec4& texturePosition, const Color& tint)
  {
    rect(position, size, textureID, 1.0f, texturePosition, tint);
  }

  void Renderer::rect(const Vector& position, const Vector& size, uint32_t textureID, float tilingFactor, const glm::vec4& texturePosition, const Color& tint)
  {
    // TODO for now assume that position is the left most corner
    quad(
      { position.x, position.y },
      { position.x + size.x, position.y },
      { position.x + size.x, position.y + size.y },
      { position.x, position.y + size.y },
      textureID, tilingFactor, texturePosition, tint);
  }

  void Renderer::rect(const Vector& position, const Vector& size, float rotation, const Color& color)
  {
    rect(position, size, rotation, rendererData.whiteTexture, 1.0f, { 0.0f, 0.0f, 1.0f, 1.0f }, color);
  }

  void Renderer::rect(const Vector& position, const Vector& size, float rotation, uint32_t textureID, float tilingFactor, const Color& tint)
  {
    rect(position, size, rotation, textureID, tilingFactor, { 0.0f, 0.0f, 1.0f, 1.0f }, tint);
  }

  void Renderer::rect(const Vector& position, const Vector& size, float rotation, uint32_t textureID, const glm::vec4& texturePosition, const Color& tint)
  {
    rect(position, size, rotation, textureID, 1.0f, texturePosition, tint);
  }

  void Renderer::rect(const Vector& position, const Vector& size, float rotation, uint32_t textureID, float tilingFactor, const glm::vec4& texturePosition, const Color& tint)
  {
    // TODO for now assume that the rotation is around the corner of the rectangle
    Transform tf(position, rotation);
    quad(
      transform({ position.x, position.y }, tf),
      transform({ position.x + size.x, position.y }, tf),
      transform({ position.x + size.x, position.y + size.y }, tf),
      transform({ position.x, position.y + size.y }, tf),
      textureID, tilingFactor, texturePosition, tint);
  }

  void Renderer::quad(const Vector& a, const Vector& b, const Vector& c, const Vector& d, const Color& color)
  {
    quad(a, b, c, d, rendererData.whiteTexture, 1.0f, { 0.0f, 0.0f, 1.0f, 1.0f }, color);
  }

  void Renderer::quad(const Vector& a, const Vector& b, const Vector& c, const Vector& d, uint32_t textureID, float tilingFactor, const Color& tint)
  {
    quad(a, b, c, d, textureID, tilingFactor, { 0.0f, 0.0f, 1.0f, 1.0f }, tint);
  }

  void Renderer::quad(const Vector& a, const Vector& b, const Vector& c, const Vector& d, uint32_t textureID, const glm::vec4& texturePosition, const Color& tint)
  {
    quad(a, b, c, d, textureID, 1.0f, texturePosition, tint);
  }

  void Renderer::quad(const Vector& a, const Vector& b, const Vector& c, const Vector& d, uint32_t textureID, float tilingFactor, const glm::vec4& texturePosition, const Color& tint)
  {
    if (rendererData.textureIndexCount + 6 >= MaxIndexCount || rendererData.textureSlotIndex >= MaxTextureSlots)
    {
      endTextureBatch();
      beginTextureBatch();
    }

    const std::array<glm::vec2, 4> vertices = { { { a.x, a.y }, { b.x, b.y }, { c.x, c.y }, { d.x, d.y } } };
    const std::array<glm::vec2, 4> textureCoords = { { 
      { texturePosition.x, texturePosition.y }, 
      { texturePosition.x + texturePosition.p, texturePosition.y }, 
      { texturePosition.x + texturePosition.p, texturePosition.y + texturePosition.q }, 
      { texturePosition.x, texturePosition.y + texturePosition.q } } };
    const glm::vec4 color = { tint.r, tint.g, tint.b, tint.a };

    float textureIndex = 0.0f;
    for (uint32_t i = 1; i < rendererData.textureSlotIndex; i++)
    {
      if (rendererData.textureSlots[i] == textureID)
      {
        textureIndex = (float)i;
        break;
      }
    }

    if (textureIndex == 0.0f)
    {
      textureIndex = (float)rendererData.textureSlotIndex;
      rendererData.textureSlots[rendererData.textureSlotIndex] = textureID;
      rendererData.textureSlotIndex++;
    }

    for (uint32_t i = 0; i < 4; i++)
    {
      rendererData.textureBufferPointer->position = vertices[i];
      rendererData.textureBufferPointer->color = color;
      rendererData.textureBufferPointer->textureCoord = textureCoords[i];
      rendererData.textureBufferPointer->textureIndex = textureIndex;
      rendererData.textureBufferPointer->tilingFactor = tilingFactor;
      rendererData.textureBufferPointer++;
    }

    rendererData.textureIndices[rendererData.textureIndexCount++] = rendererData.textureOffset + 0;
    rendererData.textureIndices[rendererData.textureIndexCount++] = rendererData.textureOffset + 1;
    rendererData.textureIndices[rendererData.textureIndexCount++] = rendererData.textureOffset + 2;
    rendererData.textureIndices[rendererData.textureIndexCount++] = rendererData.textureOffset + 2;
    rendererData.textureIndices[rendererData.textureIndexCount++] = rendererData.textureOffset + 3;
    rendererData.textureIndices[rendererData.textureIndexCount++] = rendererData.textureOffset + 0;
    rendererData.textureIndicesPointer += 6;

    rendererData.textureOffset += 4;

    rendererData.statistics.textureCalls++;
  }

  void Renderer::triangle(const Vector& a, const Vector& b, const Vector& c, const Color& color)
  {
    polygon({ a, b, c }, { 0u, 1u, 2u }, color);
  }

  void Renderer::polygon(const std::vector<Vector>& vertices, const std::vector<size_t>& triangles, const Color& color)
  {
    int numTriangles = (vertices.size() - 2) * 3;

    if (rendererData.textureIndexCount + numTriangles >= MaxIndexCount)
    {
      endTextureBatch();
      beginTextureBatch();
    }

    constexpr glm::vec2 textureCoord(0.0f, 0.0f);
    const float whiteTexture = 0.0f;
    const float tilingFactor = 1.0f;

    for (int i = 0; i < vertices.size(); i++)
    {
      rendererData.textureBufferPointer->position = { vertices[i].x, vertices[i].y };
      rendererData.textureBufferPointer->color = { color.r, color.g, color.b, color.a };
      rendererData.textureBufferPointer->textureCoord = textureCoord;
      rendererData.textureBufferPointer->textureIndex = whiteTexture;
      rendererData.textureBufferPointer->tilingFactor = tilingFactor;
      rendererData.textureBufferPointer++;
    }

    for (int i = 0; i < numTriangles; i++)
    {
      rendererData.textureIndices[rendererData.textureIndexCount++] = triangles[i] + rendererData.textureOffset;
      rendererData.textureIndicesPointer++;
    }
    rendererData.textureOffset += vertices.size();

    rendererData.statistics.textureCalls++;
  }

  void Renderer::line(const Vector& a, const Vector& b, const Color& color)
  {
    line(a, b, 1.0f, color);
  }

  void Renderer::line(const Vector& a, const Vector& b, float thickness, const Color& color)
  {
    if (rendererData.lineIndexCount + 2 >= MaxIndexCount)
    {
      endLineBatch();
      beginLineBatch();
    }

    rendererData.lineBufferPointer->position = { a.x, a.y };
    rendererData.lineBufferPointer->color = { color.r, color.g, color.b, color.a };
    rendererData.lineBufferPointer->thickness = thickness;
    rendererData.lineBufferPointer++;

    rendererData.lineBufferPointer->position = { b.x, b.y };
    rendererData.lineBufferPointer->color = { color.r, color.g, color.b, color.a };
    rendererData.lineBufferPointer->thickness = thickness;
    rendererData.lineBufferPointer++;

    rendererData.lineIndexCount += 2;

    rendererData.statistics.lineCalls++;
  }

  void Renderer::point(const Vector& position, const Color& color)
  {
    circle(position, 1.0f, 1.0f, 0.005f, color);
  }

  void Renderer::circle(const Vector& center, float radius, const Color& color)
  {
    circle(center, radius, 1.0f, 0.005f, color);
  }

  void Renderer::circle(const Vector& center, float radius, float thickness, const Color& color)
  {
    circle(center, radius, thickness, 0.005f, color);
  }

  void Renderer::circle(const Vector& center, float radius, float thickness, float fade, const Color& color)
  {
    ellipse(
      { center.x - radius, center.y - radius }, 
      { center.x + radius, center.y - radius }, 
      { center.x + radius, center.y + radius }, 
      { center.x - radius, center.y + radius },
      thickness, fade, color);
  }

  void Renderer::ellipse(const Vector& center, const Vector& size, const Color& color)
  {
    ellipse(center, size, 1.0f, 0.005f, color);
  }
  
  void Renderer::ellipse(const Vector& center, const Vector& size, float thickness, const Color& color)
  {
    ellipse(center, size, thickness, 0.005f, color);
  }

  void Renderer::ellipse(const Vector& center, const Vector& size, float thickness, float fade, const Color& color)
  {
    ellipse(
      { center.x - size.x, center.y - size.y }, 
      { center.x + size.x, center.y - size.y }, 
      { center.x + size.x, center.y + size.y }, 
      { center.x - size.x, center.y + size.y },
      thickness, fade, color);
  }

  // void Renderer::ellipse(const Vector& center, const Vector& size, float rotation, const Color& color)
  // {
  //   ellipse(center, size, rotation, 1.0f, 0.005f, color);
  // }

  // void Renderer::ellipse(const Vector& center, const Vector& size, float rotation, float thickness, const Color& color)
  // {
  //   ellipse(center, size, rotation, thickness, 0.005f, color);
  // }

  void Renderer::ellipse(const Vector& center, const Vector& size, float rotation, float thickness, float fade, const Color& color)
  {
    Transform tf(center, rotation);
    ellipse(
      transform({ center.x - size.x, center.y - size.y }, tf),
      transform({ center.x + size.x, center.y - size.y }, tf),
      transform({ center.x + size.x, center.y + size.y }, tf),
      transform({ center.x - size.x, center.y + size.y }, tf),
      thickness, fade, color);
  }

  void Renderer::ellipse(const Vector& a, const Vector& b, const Vector& c, const Vector& d, const Color& color)
  {
    ellipse(a, b, c, d, 1.0f, 0.005f, color);
  }

  void Renderer::ellipse(const Vector& a, const Vector& b, const Vector& c, const Vector& d, float thickness, const Color& color)
  {
    ellipse(a, b, c, d, thickness, 0.005f, color);
  }

  void Renderer::ellipse(const Vector& a, const Vector& b, const Vector& c, const Vector& d, float thickness, float fade, const Color& color)
  {
    if (rendererData.circleIndexCount + 6 >= MaxIndexCount)
    {
      endCircleBatch();
      beginCircleBatch();
    }

    const std::array<glm::vec2, 4> vertices = { { 
      { a.x, a.y },
      { b.x, b.y },
      { c.x, c.y },
      { d.x, d.y } } };

    for (size_t i = 0; i < vertices.size(); i++)
    {
      rendererData.circleBufferPointer->worldPosition = vertices[i];
      rendererData.circleBufferPointer->localPosition = rendererData.circleVertexPositions[i];
      rendererData.circleBufferPointer->color = { color.r, color.g, color.b, color.a };
      rendererData.circleBufferPointer->thickness = thickness;
      rendererData.circleBufferPointer->fade = fade;
      rendererData.circleBufferPointer++;
    }

    rendererData.circleIndexCount += 6;

    rendererData.statistics.circleCalls++;
  }

  void Renderer::text(Ref<FontAtlas>& atlas, const Vector& position, const std::string& text, float scale, const Color& color)
  {
    std::stringstream ss(text);
    std::string line;

    if (text.size() == 0.0f)
      return;

    float lineOffset = 0.0f;

    while(std::getline(ss, line, '\n'))
    {
      glm::vec4* texturePositions = new glm::vec4[line.size()];
      GLuint texture = atlas->get(line, texturePositions);
      glm::vec2 offsets = atlas->getOffsets() * scale;

      for (float i = 0; i < line.size(); i++)
      {
        quad(
          {position.x + i * offsets.x, position.y - lineOffset}, 
          {position.x + (i + 1.0f) * offsets.x, position.y - lineOffset}, 
          {position.x + (i + 1.0f) * offsets.x, position.y - offsets.y - lineOffset}, 
          {position.x + i * offsets.x, position.y - offsets.y - lineOffset}, 
          texture, 
          texturePositions[static_cast<int>(i)],
          color
        );
      }

      delete[] texturePositions;
      lineOffset += offsets.y;
    }
  }

  Renderer::Statistics::Statistics()
    : textureCalls(0u), circleCalls(0u), lineCalls(0u) 
  {}

  size_t Renderer::Statistics::textureDrawCalls() const { return textureCalls; }
  size_t Renderer::Statistics::circleDrawCalls() const { return circleCalls; }
  size_t Renderer::Statistics::lineDrawCalls() const { return lineCalls; }
  size_t Renderer::Statistics::totalDrawCalls() const { return textureCalls + circleCalls + lineCalls; }

  float Renderer::Statistics::texturePercentage() const { return (float)textureCalls / (float)totalDrawCalls(); }
  float Renderer::Statistics::circlePercentage() const { return (float)circleCalls / (float)totalDrawCalls(); }
  float Renderer::Statistics::linePercentage() const { return (float)lineCalls / (float)totalDrawCalls(); }

  void Renderer::Statistics::reset()
  {
    textureCalls = 0u;
    circleCalls = 0u;
    lineCalls = 0u;
  }

  Renderer::Statistics& Renderer::statistics() { return rendererData.statistics; }

}
