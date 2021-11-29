#include <flectron/renderer.hpp>

#include <exception>
#include <fstream>
#include <sstream>
#include <iostream>

#include <flectron/body.hpp>
#include <flectron/random.hpp>
#include <flectron/profile.hpp>
#include <stb_image.h>
#include <glm/gtc/type_ptr.hpp>

namespace flectron
{

  BaseShader::~BaseShader()
  {
    glDeleteProgram(rendererID);
  }

  void BaseShader::bind() const
  {
    glUseProgram(rendererID);
  }

  void BaseShader::unbind() const
  {
    glUseProgram(0);
  }

  GLuint BaseShader::getRendererID() const
  {
    return rendererID;
  }

  void BaseShader::setUniform1i(const std::string& name, int value)
  {
    glUniform1i(getUniformLocation(name), value);
  }

  void BaseShader::setUniform1f(const std::string& name, float value)
  {
    glUniform1f(getUniformLocation(name), value);
  }
  
  void BaseShader::setUniform2f(const std::string& name, float v1, float v2)
  {
    glUniform2f(getUniformLocation(name), v1, v2);
  }

  void BaseShader::setUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
  {
    glUniform4f(getUniformLocation(name), v0, v1, v2, v3);
  }

  void BaseShader::setUniformMat4f(const std::string& name, const glm::mat4& matrix)
  {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
  }

  void BaseShader::setUniform1iv(const std::string& name, int* array, int size)
  {
    glUniform1iv(getUniformLocation(name), size, array);
  }

  void BaseShader::setUniform2fv(const std::string& name, float* array, int size)
  {
    glUniform2fv(getUniformLocation(name), size, array);
  }

  void BaseShader::setUniform3fv(const std::string& name, float* array, int size)
  {
    glUniform3fv(getUniformLocation(name), size, array);
  }

  void BaseShader::setUniform4fv(const std::string& name, float* array, int size)
  {
    glUniform4fv(getUniformLocation(name), size, array);
  }

  int BaseShader::getUniformLocation(const std::string& name)
  {
    if (locationCache.find(name) != locationCache.end())
      return locationCache[name];

    int location = glGetUniformLocation(this->rendererID, name.c_str());
    if (location == -1)
      throw std::runtime_error("Warning: uniform " + name + " doesn't exist!\n");
    
    locationCache[name] = location;
    return location;
  }

  std::string BaseShader::getSource(const std::string& filepath) const
  {
    std::ifstream file(filepath);

    if (!file.is_open())
      throw std::runtime_error("Error: could not open file " + filepath + "\n");
    
    std::string source;
    std::string line;
    while (std::getline(file, line))
      source += line + "\n";
    
    return source;
  }

  unsigned int BaseShader::compileShader(unsigned int type, const std::string& source)
  {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
      int length;
      glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
      char* message = (char*)alloca(length * sizeof(char));
      glGetShaderInfoLog(id, length, &length, message);
      std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!\n";
      std::cout << message << "\n";
      glDeleteShader(id);
      return 0;
    }

    return id;
  }

  Shader::Shader(const std::string& filepathVertex, const std::string& filepathFragment)
    : filepathVertex(filepathVertex), filepathFragment(filepathFragment)
  {
    std::string vertexShader = getSource(filepathVertex);
    std::string fragmentShader = getSource(filepathFragment);

    rendererID = glCreateProgram();
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(rendererID, vs);
    glAttachShader(rendererID, fs);
    glLinkProgram(rendererID);
    glValidateProgram(rendererID);

    glDeleteShader(vs);
    glDeleteShader(fs);
  }

  ComputeShader::ComputeShader(const std::string& filepath)
    : filepath(filepath)
  {
    std::string source = getSource(filepath);

    rendererID = glCreateProgram();
    unsigned int cs = compileShader(GL_COMPUTE_SHADER, source);

    glAttachShader(rendererID, cs);
    glLinkProgram(rendererID);
    glValidateProgram(rendererID);

    glDeleteShader(cs);

    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &maxWorkGroupCount[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &maxWorkGroupCount[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &maxWorkGroupCount[2]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &maxWorkGroupSize[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &maxWorkGroupSize[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &maxWorkGroupSize[2]);
    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &maxWorkGroupInvocations);
  }

  void ComputeShader::dispatch(int x, int y, int z) const
  {
    glDispatchCompute((GLuint)x, (GLuint)y, (GLuint)z);
  }

  void ComputeShader::barrier() const
  {
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  }

  Texture::Texture(const std::string& filepath, bool nearest, bool repeat)
    : rendererID(loadTexture(filepath, nearest, repeat)), filepath(filepath)
  {}

  Texture::~Texture()
  {
    glDeleteTextures(1, &rendererID);
  }

  GLuint Texture::get() const
  {
    return rendererID;
  }

  TextureAtlas::TextureAtlas(const std::string& filepath, int columns, int rows, bool nearest)
    : Texture(filepath, nearest, false), columns(columns), rows(rows)
  {
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

  AnimationRange::AnimationRange(float x, float y, float width, float height, float unit, size_t interval, Animation* animation)
    : x(x), y(y), width(width), height(height), unit(unit), total((size_t)(width * height)), interval(interval), animation(animation)
  {}

  glm::vec4* AnimationRange::getNext(AnimationState& state)
  {
    glm::vec4* frame = animation->atlas->frames[state.currentName][state.currentRange][state.currentFrame].get();

    if (state.currentTick == interval)
    {
      state.currentTick = 0;
      state.currentFrame++;
    }
    else
      state.currentTick++;

    if (state.currentFrame == total)
    {
      state.currentFrame = 0;
      state.currentRange++;
    }

    return frame;
  }

  Animation::Animation(const std::string& name, AnimationAtlas* atlas)
    : ranges(), possibleFutureAnimations(), atlas(atlas), name(name)
  {}

  void Animation::addRange(float x, float y, float width, float height, float unit, size_t interval)
  {
    ranges.push_back(createRef<AnimationRange>(x, y, width, height, unit, interval, this));
  }

  glm::vec4* Animation::getNext(AnimationState& state)
  {
    if (ranges.size() == 0)
      return nullptr;

    glm::vec4* frame = ranges[state.currentRange]->getNext(state);

    if (state.currentRange == ranges.size())
    {
      for (auto& range : possibleFutureAnimations)
      {
        float r = randomFloat(0.0f, 1.0f);
        if (r < range.second)
        {
          state.currentName = range.first;
          break;
        }
      }
      state.currentRange = 0;
    }

    return frame;
  }

  AnimationAtlas::AnimationAtlas(const std::string& filepath, int columns, int rows, bool nearest, const std::string& descriptionFilepath) 
    : TextureAtlas(filepath, columns, rows, nearest), animations(), frames()
  {
    std::ifstream file(descriptionFilepath);
    std::string line;
    std::string name;
    std::string position;
    std::string size;
    std::string sinterval;
    std::string sx, sy, sw, sh;
    float x, y, w, h;
    int interval;

    std::getline(file, line);
    if (line != "RANGES")
      throw std::runtime_error("Invalid description file!");

    while (std::getline(file, line))
    {
      if (line == "CONNECTIONS")
        break;

      std::istringstream ss(line);
      std::getline(ss, name, ' ');
      std::getline(ss, position, ' ');
      std::istringstream ss2(position);
      std::getline(ss2, sx, ';');
      std::getline(ss2, sy, ';');
      x = std::stof(sx);
      y = std::stof(sy);
      std::getline(ss, size, ' ');
      std::istringstream ss3(size);
      std::getline(ss3, sw, ';');
      std::getline(ss3, sh, ';');
      w = std::stof(sw);
      h = std::stof(sh);
      std::getline(ss, sinterval, ' ');
      interval = std::stoi(sinterval);

      if (animations.count(name) == 0)
        animations[name] = createRef<Animation>(name, this);
      
      animations[name]->addRange(x, y, w, h, 1.0f, (size_t)interval);

      glm::vec4 frame;
      int counter = 0;
      for (float j = y; j < y+h; j++)
        for (float i = x; i < x+w; i++)
        {
          this->get(i, j, 1.0f, 1.0f, frame);
          if (frames.count(name) == 0)
          {
            frames[name] = { { createRef<glm::vec4>(frame) } };
          }
          else
          {
            if (counter == 0)
            {
              frames[name].push_back( { createRef<glm::vec4>(frame) } );
            }
            else
            {
              frames[name][frames[name].size() - 1].push_back(createRef<glm::vec4>(frame));
            }
          }
          counter++;
        }
    }

    std::string nameA, nameB, sweight;
    float weight;
    while (std::getline(file, line))
    {
      std::istringstream ss(line);

      std::getline(ss, nameA, ' ');
      std::getline(ss, nameB, ' ');
      std::getline(ss, sweight, ' ');
      weight = std::stof(sweight);

      if (animations.count(nameA) == 0 || animations.count(nameB) == 0)
        throw std::runtime_error("Invalid description file!");

      animations[nameA]->possibleFutureAnimations[nameB] = weight;
    }
  }

  Animation* AnimationAtlas::getAnimation(const std::string& name)
  {
    if (animations.count(name) == 0)
      throw std::runtime_error("Animation not found!");
    
    return animations[name].get();
  }

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

  void Renderer::draw(const Vector* vertices, int numVertices, const uint32_t* triangles, const Color& color)
  {
    int numTriangles = (numVertices - 2) * 3;

    if (rendererData.indexCount + numTriangles >= MaxIndexCount)
    {
      endBatch();
      beginBatch();
    }

    constexpr glm::vec2 textureCoord(0.0f, 0.0f);
    const float whiteTexture = 0.0f;
    const float tilingFactor = 1.0f;

    for (int i = 0; i < numVertices; i++)
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
    rendererData.offset += numVertices;
  
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

  GLuint loadTexture(const std::string& filepath, bool nearest, bool repeat)
  {
    int w, h, bits;
    // stbi_set_flip_vertically_on_load(true);
    auto* pixels = stbi_load(filepath.c_str(), &w, &h, &bits, STBI_rgb_alpha);
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

}
