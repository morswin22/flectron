#pragma once

#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <flectron/utils/memory.hpp>
#include <flectron/renderer/shader.hpp>
#include <flectron/renderer/texture.hpp>
#include <flectron/renderer/color.hpp>
#include <flectron/application/camera.hpp>

namespace flectron
{

  struct Vector;
  struct Color;
  struct AABB;

  class Renderer
  {
  private:
    static void initTextureRendering();
    static void initCircleRendering();
    static void initLineRendering();

  public:
    static void init(int width, int height, GLuint& buffer);
    static void shutdown();
    static void setViewProjectionMatrix(const Camera& camera);

    static void beginBatch();
    static void endBatch();

  private:
    static void beginTextureBatch();
    static void beginCircleBatch();
    static void beginLineBatch();
    static void endTextureBatch();
    static void endCircleBatch();
    static void endLineBatch();

  public:
    static void onscreen();
    static void offscreen();

    // Square
    static void square(const Vector& position, float size, const Color& color = Colors::white());
    static void square(const Vector& position, float size, uint32_t textureID, float tilingFactor, const Color& tint = Colors::white());
    static void square(const Vector& position, float size, uint32_t textureID, const glm::vec4& texturePosition, const Color& tint = Colors::white());
    static void square(const Vector& position, float size, uint32_t textureID, float tilingFactor, const glm::vec4& texturePosition, const Color& tint = Colors::white());

    // Rotated square
    static void square(const Vector& position, float size, float rotation, const Color& color = Colors::white());
    static void square(const Vector& position, float size, float rotation, uint32_t textureID, float tilingFactor, const Color& tint = Colors::white());
    static void square(const Vector& position, float size, float rotation, uint32_t textureID, const glm::vec4& texturePosition, const Color& tint = Colors::white());
    static void square(const Vector& position, float size, float rotation, uint32_t textureID, float tilingFactor, const glm::vec4& texturePosition, const Color& tint = Colors::white());
    
    // Rectangle
    static void rect(const Vector& position, const Vector& size, const Color& color = Colors::white());
    static void rect(const Vector& position, const Vector& size, uint32_t textureID, float tilingFactor, const Color& tint = Colors::white());
    static void rect(const Vector& position, const Vector& size, uint32_t textureID, const glm::vec4& texturePosition, const Color& tint = Colors::white());
    static void rect(const Vector& position, const Vector& size, uint32_t textureID, float tilingFactor, const glm::vec4& texturePosition, const Color& tint = Colors::white());
    
    // Rotated rectangle
    static void rect(const Vector& position, const Vector& size, float rotation, const Color& color = Colors::white());
    static void rect(const Vector& position, const Vector& size, float rotation, uint32_t textureID, float tilingFactor, const Color& tint = Colors::white());
    static void rect(const Vector& position, const Vector& size, float rotation, uint32_t textureID, const glm::vec4& texturePosition, const Color& tint = Colors::white());
    static void rect(const Vector& position, const Vector& size, float rotation, uint32_t textureID, float tilingFactor, const glm::vec4& texturePosition, const Color& tint = Colors::white());
    
    // Quad
    static void quad(const Vector& a, const Vector& b, const Vector& c, const Vector& d, const Color& color = Colors::white());
    static void quad(const Vector& a, const Vector& b, const Vector& c, const Vector& d, uint32_t textureID, float tilingFactor, const Color& tint = Colors::white());
    static void quad(const Vector& a, const Vector& b, const Vector& c, const Vector& d, uint32_t textureID, const glm::vec4& texturePosition, const Color& tint = Colors::white());
    static void quad(const Vector& a, const Vector& b, const Vector& c, const Vector& d, uint32_t textureID, float tilingFactor, const glm::vec4& texturePosition, const Color& tint = Colors::white());
  
    // Polygon
    static void triangle(const Vector& a, const Vector& b, const Vector& c, const Color& color = Colors::white());
    static void polygon(const std::vector<Vector>& vertices, const std::vector<size_t>& triangles, const Color& color = Colors::white());
  
    // Line
    static void line(const Vector& a, const Vector& b, const Color& color = Colors::white());
    static void line(const Vector& a, const Vector& b, float thickness, const Color& color = Colors::white());
    static void debugLine(const Vector& a, const Vector& b, const Color& color = Colors::white());
    static void debugLineWidth(float width);

    // Circle
    static void point(const Vector& position, const Color& color = Colors::white());
    static void circle(const Vector& center, float radius, const Color& color = Colors::white());
    static void circle(const Vector& center, float radius, float thickness, const Color& color = Colors::white());
    static void circle(const Vector& center, float radius, float thickness, float fade, const Color& color = Colors::white());

    // Ellipse
    static void ellipse(const Vector& center, const Vector& size, const Color& color = Colors::white());
    static void ellipse(const Vector& center, const Vector& size, float thickness, const Color& color = Colors::white());
    static void ellipse(const Vector& center, const Vector& size, float thickness, float fade, const Color& color = Colors::white());

    // Rotated ellipse
    // TODO fix the overlapping overloads
    // static void ellipse(const Vector& center, const Vector& size, float rotation, const Color& color = Colors::white()); 
    // static void ellipse(const Vector& center, const Vector& size, float rotation, float thickness, const Color& color = Colors::white());
    static void ellipse(const Vector& center, const Vector& size, float rotation, float thickness, float fade, const Color& color = Colors::white());

    // Quad-defined ellipse
    static void ellipse(const Vector& a, const Vector& b, const Vector& c, const Vector& d, const Color& color = Colors::white());
    static void ellipse(const Vector& a, const Vector& b, const Vector& c, const Vector& d, float thickness, const Color& color = Colors::white());
    static void ellipse(const Vector& a, const Vector& b, const Vector& c, const Vector& d, float thickness, float fade, const Color& color = Colors::white());

    // Text
    static void text(Ref<FontAtlas>& atlas, const Vector& position, const std::string& text, float scale, const Color& color = Colors::white());
  };

}
