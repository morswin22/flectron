#pragma once

#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <string>
#include <unordered_map>

#include <flectron/assets/image.hpp>

namespace flectron
{
  
  GLuint createFrameBuffer(int width, int height, GLuint& buffer);

  class TextureAtlas
  {
  public:
    ImageView image;

  protected:
    int columns;
    int rows;
    float xOffset;
    float yOffset;

  public:
    TextureAtlas(const Image& image, int columns, int rows);
    TextureAtlas(const ImageView& image, int columns, int rows);

    TextureAtlas(const TextureAtlas&) = delete;
    TextureAtlas& operator=(const TextureAtlas&) = delete;

    TextureAtlas(TextureAtlas&&) = delete;
    TextureAtlas& operator=(TextureAtlas&&) = delete;

    GLuint get(float column, float row, float width, float height, glm::vec4& texturePosition) const;
  };

  class FontAtlas : public TextureAtlas
  {
  private:
    std::string alphabet;
    std::unordered_map<char, int> indexMap;

  public:
    FontAtlas(const Image& image, int columns, int rows, const std::string& alphabet);
    FontAtlas(const ImageView& image, int columns, int rows, const std::string& alphabet);

    FontAtlas(const FontAtlas&) = delete;
    FontAtlas& operator=(const FontAtlas&) = delete;

    FontAtlas(FontAtlas&&) = delete;
    FontAtlas& operator=(FontAtlas&&) = delete;

    GLuint get(const std::string& text, glm::vec4* texturePositions);
    glm::vec2 getOffsets() const;
  };

}
