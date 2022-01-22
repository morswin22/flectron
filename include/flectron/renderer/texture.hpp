#pragma once

#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <string>
#include <unordered_map>

namespace flectron
{
  
  GLuint loadTexture(const std::string& filepath, bool nearest, bool repeat);
  GLuint createFrameBuffer(int width, int height, GLuint& buffer);

  class Texture
  {
  protected:
    GLuint rendererID;
    std::string filepath;
  
  public:
    Texture(const std::string& filepath, bool nearest, bool repeat);
    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    Texture(Texture&&) = delete;
    Texture& operator=(Texture&&) = delete;

    GLuint get() const;
  };

  class TextureAtlas : public Texture
  {
  protected:
    int columns;
    int rows;
    float xOffset;
    float yOffset;

  public:
    TextureAtlas(const std::string& filepath, int columns, int rows, bool nearest);

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
    FontAtlas(const std::string& filepath, int columns, int rows, const std::string& alphabet);

    FontAtlas(const FontAtlas&) = delete;
    FontAtlas& operator=(const FontAtlas&) = delete;

    FontAtlas(FontAtlas&&) = delete;
    FontAtlas& operator=(FontAtlas&&) = delete;

    GLuint get(const std::string& text, glm::vec4* texturePositions);
    glm::vec2 getOffsets() const;
  };

}
