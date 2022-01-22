#include <flectron/renderer/renderer.hpp>

#include <exception>
#include <fstream>
#include <sstream>
#include <iostream>

#include <flectron/renderer/color.hpp>
#include <flectron/physics/vector.hpp>
#include <flectron/utils/random.hpp>
#include <flectron/utils/profile.hpp>
#include <stb_image.h>
#include <glm/gtc/type_ptr.hpp>

namespace flectron
{

  static const std::size_t MaxTriangleCount = 10000;
  static const std::size_t MaxVertexCount = MaxTriangleCount * 3;
  static const std::size_t MaxIndexCount = MaxTriangleCount * 3;
  static size_t MaxTextureSlots;

  struct Vertex
  {
    glm::vec2 position;
    glm::vec4 color;
    glm::vec2 textureCoord;
    float textureIndex;
    float tilingFactor;
  };

  struct RendererData
  {
    GLuint va = 0;
    GLuint vb = 0;
    GLuint ib = 0;

    Vertex* buffer = nullptr;
    Vertex* bufferPointer = nullptr;

    uint32_t* indices = nullptr;
    uint32_t* indicesPointer = nullptr;

    uint32_t indexCount = 0;
    uint32_t offset = 0;

    GLuint whiteTexture = 0;

    uint32_t* textureSlots = nullptr;
    uint32_t textureSlotIndex = 1;

    Ref<Shader> shader = nullptr;
    GLuint frameBuffer = 0;
  };

  static RendererData rendererData;

  void Renderer::init(Ref<Shader>& shader, int width, int height, GLuint& buffer)
  {
    rendererData.buffer = new Vertex[MaxVertexCount];

    glCreateVertexArrays(1, &rendererData.va);
    glBindVertexArray(rendererData.va);

    glCreateBuffers(1, &rendererData.vb);
    glBindBuffer(GL_ARRAY_BUFFER, rendererData.vb);
    glBufferData(GL_ARRAY_BUFFER, MaxVertexCount * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexArrayAttrib(rendererData.va, 0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));

    glEnableVertexArrayAttrib(rendererData.va, 1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, color));

    glEnableVertexArrayAttrib(rendererData.va, 2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, textureCoord));

    glEnableVertexArrayAttrib(rendererData.va, 3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, textureIndex));

    glEnableVertexArrayAttrib(rendererData.va, 4);
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, tilingFactor));

    rendererData.indices = new uint32_t[MaxIndexCount];

    glGenBuffers(1, &rendererData.ib);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererData.ib);
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

    shader->bind();
    shader->setUniform1iv("uTextures", samplers, tempMaxTextureSlots);
    
    delete[] samplers;

    rendererData.shader = shader;

    rendererData.frameBuffer = createFrameBuffer(width, height, buffer);
  }

  void Renderer::shutdown()
  {
    glDeleteVertexArrays(1, &rendererData.va);
    glDeleteBuffers(1, &rendererData.vb);
    glDeleteBuffers(1, &rendererData.ib);
    glDeleteTextures(1, &rendererData.whiteTexture);
    if (rendererData.frameBuffer != 0)
      glDeleteFramebuffers(1, &rendererData.frameBuffer);

    delete[] rendererData.buffer;
    delete[] rendererData.indices;
    delete[] rendererData.textureSlots;
  }

  void Renderer::beginBatch()
  {
    rendererData.bufferPointer = rendererData.buffer;
    rendererData.indicesPointer = rendererData.indices;
    rendererData.indexCount = 0;
    rendererData.offset = 0;
    rendererData.textureSlotIndex = 1;
  }

  void Renderer::endBatch()
  {
    rendererData.shader->bind();

    GLsizeiptr quadSize = (uint8_t*)rendererData.bufferPointer - (uint8_t*)rendererData.buffer;
    glBindBuffer(GL_ARRAY_BUFFER, rendererData.vb);
    glBufferSubData(GL_ARRAY_BUFFER, 0, quadSize, rendererData.buffer);

    GLsizeiptr indexSize = (uint8_t*)rendererData.indicesPointer - (uint8_t*)rendererData.indices;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererData.ib);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indexSize, rendererData.indices);

    for (uint32_t i = 0; i < rendererData.textureSlotIndex; i++)
      glBindTextureUnit(i, rendererData.textureSlots[i]);

    glBindVertexArray(rendererData.va);
    glDrawElements(GL_TRIANGLES, rendererData.indexCount, GL_UNSIGNED_INT, nullptr);
  }

  void Renderer::onscreen()
  {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  void Renderer::offscreen()
  {
    glBindFramebuffer(GL_FRAMEBUFFER, rendererData.frameBuffer);
  }

  void Renderer::draw(const std::vector<Vector>& vertices, const std::vector<size_t>& triangles, const Color& color)
  {
    int numTriangles = (vertices.size() - 2) * 3;

    if (rendererData.indexCount + numTriangles >= MaxIndexCount)
    {
      endBatch();
      beginBatch();
    }

    constexpr glm::vec2 textureCoord(0.0f, 0.0f);
    const float whiteTexture = 0.0f;
    const float tilingFactor = 1.0f;

    for (int i = 0; i < vertices.size(); i++)
    {
      rendererData.bufferPointer->position = { vertices[i].x, vertices[i].y };
      rendererData.bufferPointer->color = { color.r, color.g, color.b, color.a };
      rendererData.bufferPointer->textureCoord = textureCoord;
      rendererData.bufferPointer->textureIndex = whiteTexture;
      rendererData.bufferPointer->tilingFactor = tilingFactor;
      rendererData.bufferPointer++;
    }

    for (int i = 0; i < numTriangles; i++)
    {
      rendererData.indices[rendererData.indexCount++] = triangles[i] + rendererData.offset;
      rendererData.indicesPointer++;
    }
    rendererData.offset += vertices.size();
  }

  void Renderer::draw(const Vector& a, const Vector& b, const Vector& c, const Vector& d, uint32_t textureID, float tilingFactor, const Color& tint)
  {
    if (rendererData.indexCount + 6 >= MaxIndexCount || rendererData.textureSlotIndex >= MaxTextureSlots)
    {
      endBatch();
      beginBatch();
    }

    const glm::vec4 color( tint.r, tint.g, tint.b, tint.a );

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

    rendererData.bufferPointer->position = { a.x, a.y };
    rendererData.bufferPointer->color = color;
    rendererData.bufferPointer->textureCoord = { 0.0f, 0.0f };
    rendererData.bufferPointer->textureIndex = textureIndex;
    rendererData.bufferPointer->tilingFactor = tilingFactor;
    rendererData.bufferPointer++;

    rendererData.bufferPointer->position = { b.x, b.y };
    rendererData.bufferPointer->color = color;
    rendererData.bufferPointer->textureCoord = { 1.0f, 0.0f };
    rendererData.bufferPointer->textureIndex = textureIndex;
    rendererData.bufferPointer->tilingFactor = tilingFactor;
    rendererData.bufferPointer++;

    rendererData.bufferPointer->position = { c.x, c.y };
    rendererData.bufferPointer->color = color;
    rendererData.bufferPointer->textureCoord = { 1.0f, 1.0f };
    rendererData.bufferPointer->textureIndex = textureIndex;
    rendererData.bufferPointer->tilingFactor = tilingFactor;
    rendererData.bufferPointer++;

    rendererData.bufferPointer->position = { d.x, d.y };
    rendererData.bufferPointer->color = color;
    rendererData.bufferPointer->textureCoord = { 0.0f, 1.0f };
    rendererData.bufferPointer->textureIndex = textureIndex;
    rendererData.bufferPointer->tilingFactor = tilingFactor;
    rendererData.bufferPointer++;

    rendererData.indices[rendererData.indexCount++] = rendererData.offset + 0;
    rendererData.indices[rendererData.indexCount++] = rendererData.offset + 1;
    rendererData.indices[rendererData.indexCount++] = rendererData.offset + 2;
    rendererData.indices[rendererData.indexCount++] = rendererData.offset + 2;
    rendererData.indices[rendererData.indexCount++] = rendererData.offset + 3;
    rendererData.indices[rendererData.indexCount++] = rendererData.offset + 0;
    rendererData.indicesPointer += 6;

    rendererData.offset += 4;
  }

  void Renderer::draw(const Vector& a, const Vector& b, const Vector& c, const Vector& d, uint32_t textureID, const glm::vec4& texturePosition, const Color& tint)
  {
    if (rendererData.indexCount + 6 >= MaxIndexCount || rendererData.textureSlotIndex >= MaxTextureSlots)
    {
      endBatch();
      beginBatch();
    }

    const glm::vec4 color = { tint.r, tint.g, tint.b, tint.a };
    const float tilingFactor = 1.0f;

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

    rendererData.bufferPointer->position = { a.x, a.y };
    rendererData.bufferPointer->color = color;
    rendererData.bufferPointer->textureCoord = { texturePosition.x, texturePosition.y };
    rendererData.bufferPointer->textureIndex = textureIndex;
    rendererData.bufferPointer->tilingFactor = tilingFactor;
    rendererData.bufferPointer++;

    rendererData.bufferPointer->position = { b.x, b.y };
    rendererData.bufferPointer->color = color;
    rendererData.bufferPointer->textureCoord = { texturePosition.x + texturePosition.p, texturePosition.y };
    rendererData.bufferPointer->textureIndex = textureIndex;
    rendererData.bufferPointer->tilingFactor = tilingFactor;
    rendererData.bufferPointer++;

    rendererData.bufferPointer->position = { c.x, c.y };
    rendererData.bufferPointer->color = color;
    rendererData.bufferPointer->textureCoord = { texturePosition.x + texturePosition.p, texturePosition.y + texturePosition.q };
    rendererData.bufferPointer->textureIndex = textureIndex;
    rendererData.bufferPointer->tilingFactor = tilingFactor;
    rendererData.bufferPointer++;

    rendererData.bufferPointer->position = { d.x, d.y };
    rendererData.bufferPointer->color = color;
    rendererData.bufferPointer->textureCoord = { texturePosition.x, texturePosition.y + texturePosition.q };
    rendererData.bufferPointer->textureIndex = textureIndex;
    rendererData.bufferPointer->tilingFactor = tilingFactor;
    rendererData.bufferPointer++;

    rendererData.indices[rendererData.indexCount++] = rendererData.offset + 0;
    rendererData.indices[rendererData.indexCount++] = rendererData.offset + 1;
    rendererData.indices[rendererData.indexCount++] = rendererData.offset + 2;
    rendererData.indices[rendererData.indexCount++] = rendererData.offset + 2;
    rendererData.indices[rendererData.indexCount++] = rendererData.offset + 3;
    rendererData.indices[rendererData.indexCount++] = rendererData.offset + 0;
    rendererData.indicesPointer += 6;

    rendererData.offset += 4;
  }

  void Renderer::draw(const Vector& a, const Vector& b, const Vector& c, const Vector& d, TextureAtlas* textureAtlas, float x, float y, float w, float h, const Color& tint)
  {
    glm::vec4 texturePosition;
    GLuint textureID = textureAtlas->get(x, y, w, h, texturePosition);
    draw(a, b, c, d, textureID, texturePosition, tint);
  }

}
