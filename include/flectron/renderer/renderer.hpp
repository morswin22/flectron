#pragma once

#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <flectron/utils/memory.hpp>
#include <flectron/renderer/shader.hpp>
#include <flectron/renderer/texture.hpp>

namespace flectron
{

  struct Vector;
  struct Color;

  class Renderer
  {
  public:
    static void init(Ref<Shader>& shader, int width, int height, GLuint& buffer);
    static void shutdown();

    static void beginBatch();
    static void endBatch();

    static void onscreen();
    static void offscreen();

    static void draw(const std::vector<Vector>& vertices, const std::vector<size_t>& triangles, const Color& color);
    static void draw(const Vector& a, const Vector& b, const Vector& c, const Vector& d, uint32_t textureID, float tilingFactor, const Color& tint);
    static void draw(const Vector& a, const Vector& b, const Vector& c, const Vector& d, uint32_t textureID, const glm::vec4& texturePosition, const Color& tint);
    static void draw(const Vector& a, const Vector& b, const Vector& c, const Vector& d, TextureAtlas* textureAtlas, float x, float y, float w, float h, const Color& tint);
  };

}
