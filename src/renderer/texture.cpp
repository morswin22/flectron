#include <flectron/renderer/texture.hpp>

#include <flectron/assert/assert.hpp>
#include <stb_image.h>

namespace flectron
{

  GLuint loadTexture(const std::string& filepath, bool nearest, bool repeat)
  {
    FLECTRON_LOG_TRACE("Loading texture from {}", filepath);
    int w, h, bits;
    // stbi_set_flip_vertically_on_load(true);
    auto* pixels = stbi_load(filepath.c_str(), &w, &h, &bits, STBI_rgb_alpha);
    FLECTRON_ASSERT(pixels, "Failed to load texture");
    GLuint textureID;
    glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, nearest ? GL_NEAREST : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, nearest ? GL_NEAREST : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    stbi_image_free(pixels);
    return textureID;
  }

  GLuint createFrameBuffer(int width, int height, GLuint& buffer)
  {
    GLuint fbo;
    glCreateFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &buffer);
    glBindTexture(GL_TEXTURE_2D, buffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer, 0);  

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return fbo;
  }
 
  Texture::Texture(const std::string& filepath, bool nearest, bool repeat)
    : rendererID(loadTexture(filepath, nearest, repeat)), filepath(filepath)
  {
    FLECTRON_LOG_TRACE("Creating texture from {}", filepath);
  }

  Texture::~Texture()
  {
    FLECTRON_LOG_TRACE("Destroying texture {}", filepath);
    glDeleteTextures(1, &rendererID);
  }

  GLuint Texture::get() const
  {
    return rendererID;
  }

  TextureAtlas::TextureAtlas(const std::string& filepath, int columns, int rows, bool nearest)
    : Texture(filepath, nearest, false), columns(columns), rows(rows)
  {
    FLECTRON_LOG_TRACE("Creating texture atlas from {}", filepath);
    xOffset = 1.0f / (float)columns;
    yOffset = 1.0f / (float)rows;
  }

  GLuint TextureAtlas::get(float column, float row, float width, float height, glm::vec4& texturePosition) const
  {
    texturePosition.x = xOffset * column;
    texturePosition.y = yOffset * row;
    texturePosition.p = xOffset * width;
    texturePosition.q = yOffset * height;
    return rendererID;
  }

  FontAtlas::FontAtlas(const std::string& filepath, int columns, int rows, const std::string& alphabet)
    : TextureAtlas(filepath, columns, rows, true), alphabet(alphabet)
  {
    FLECTRON_LOG_TRACE("Creating font atlas from {}", filepath);
    for (size_t i = 0; i < alphabet.size(); i++)
      indexMap[(char)alphabet[i]] = (int)i;
  }

  GLuint FontAtlas::get(const std::string& text, glm::vec4* texturePositions)
  {
    for (size_t i = 0; i < text.size(); i++)
    {
      int index = indexMap[(char)text[i]];
      int x = index % columns;
      int y = index / columns;

      texturePositions[i].x = xOffset * (float)x;
      texturePositions[i].y = yOffset * (float)y;
      texturePositions[i].p = xOffset;
      texturePositions[i].q = yOffset;
    }
    return rendererID;
  }

  glm::vec2 FontAtlas::getOffsets() const
  {
    return { xOffset, yOffset };
  }

}
