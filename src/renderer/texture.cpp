#include <flectron/renderer/texture.hpp>

#include <flectron/assert/assert.hpp>
#include <stb_image.h>

namespace flectron
{

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

  TextureAtlas::TextureAtlas(const Image& image, int columns, int rows) : TextureAtlas(static_cast<ImageView>(image), columns, rows) {}

  TextureAtlas::TextureAtlas(const ImageView& image, int columns, int rows)
    : image(image), columns(columns), rows(rows)
  {
    FLECTRON_LOG_TRACE("Creating texture atlas");
    FLECTRON_LOG_DEBUG("\tSource: {} ({}x{} | {})", image->info(), image->width, image->height, image->textureID);
    xOffset = 1.0f / (float)columns;
    yOffset = 1.0f / (float)rows;
  }

  GLuint TextureAtlas::get(float column, float row, float width, float height, glm::vec4& texturePosition) const
  {
    texturePosition.x = xOffset * column;
    texturePosition.y = yOffset * row;
    texturePosition.p = xOffset * width;
    texturePosition.q = yOffset * height;
    return image->getGPU();
  }

  FontAtlas::FontAtlas(const Image& image, int columns, int rows, const std::string& alphabet) : FontAtlas(static_cast<ImageView>(image), columns, rows, alphabet) {}

  FontAtlas::FontAtlas(const ImageView& image, int columns, int rows, const std::string& alphabet)
    : TextureAtlas(image, columns, rows), alphabet(alphabet)
  {
    FLECTRON_LOG_TRACE("Creating font atlas");
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
    return image->getGPU();
  }

  glm::vec2 FontAtlas::getOffsets() const
  {
    return { xOffset, yOffset };
  }

}
