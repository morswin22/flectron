#pragma once

#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>
#include <string>
#include <unordered_map>
#include <MindlessEngine/memory.hpp>

namespace MindlessEngine
{

  class BaseShader
  {
  protected:
    GLuint rendererID;
    std::unordered_map<std::string, int> locationCache;

    int getUniformLocation(const std::string& name);

    std::string getSource(const std::string& filepath) const;
    unsigned int compileShader(unsigned int type, const std::string& source);
    unsigned int createShader(const std::string& vertexShader, const std::string& fragmentShader);

  public:
    ~BaseShader();

    void bind() const;
    void unbind() const;

    GLuint getRendererID() const;

    void setUniform1i(const std::string& name, int value);
    void setUniform1f(const std::string& name, float value);
    void setUniform2f(const std::string& name, float v1, float v2);
    void setUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
    void setUniformMat4f(const std::string& name, const glm::mat4& matrix);
    void setUniform1iv(const std::string& name, int* array, int size);
    void setUniform2fv(const std::string& name, float* array, int size);
    void setUniform3fv(const std::string& name, float* array, int size);
    void setUniform4fv(const std::string& name, float* array, int size);
  };

  class Shader : public BaseShader
  {
  private:
    std::string filepathVertex;
    std::string filepathFragment;

  public:
    Shader(const std::string& filepathVertex, const std::string& filepathFragment);
  };

  class ComputeShader : public BaseShader
  {
  private:
    std::string filepath;
    int maxWorkGroupCount[3];
    int maxWorkGroupSize[3];
    int maxWorkGroupInvocations;

  public:
    ComputeShader(const std::string& filepath);

    void dispatch(int x, int y, int z) const;
    void barrier() const;
  };

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

  class Animation;
  class AnimationRange;
  class AnimationAtlas;

  struct AnimationState
  {
    std::string currentName;
    size_t currentRange;
    size_t currentFrame;
    size_t currentTick;

    AnimationState(const std::string& name) : currentName(name), currentRange(0u), currentFrame(0u), currentTick(0u) {}
  };

  class AnimationRange
  {
  private:
    float x;
    float y;
    float width;
    float height;
    float unit;
    size_t total;
    size_t interval;
    Animation* animation;

  public:
    AnimationRange(float x, float y, float width, float height, float unit, size_t interval, Animation* animation);

    glm::vec4* getNext(AnimationState& state);
  };

  class Animation
  {
  private:
    std::vector<Ref<AnimationRange>> ranges;
    std::unordered_map<std::string, float> possibleFutureAnimations;
    AnimationAtlas* atlas;

  public:
    std::string name;

    Animation(const std::string& name, AnimationAtlas* atlas);

    void addRange(float x, float y, float width, float height, float unit, size_t interval);
    void addPossibleFutureAnimation(const std::string& animation, float probability);

    glm::vec4* getNext(AnimationState& state);

    friend AnimationAtlas;
    friend AnimationRange;
  };

  class AnimationAtlas : public TextureAtlas
  {
  private:
    std::unordered_map<std::string, Ref<Animation>> animations;
    std::unordered_map<std::string, std::vector<std::vector<Ref<glm::vec4>>>> frames;

  public:
    AnimationAtlas(const std::string& filepath, int columns, int rows, bool nearest, const std::string& descriptionFilepath);

    AnimationAtlas(const AnimationAtlas&) = delete;
    AnimationAtlas& operator=(const AnimationAtlas&) = delete;

    AnimationAtlas(AnimationAtlas&&) = delete;
    AnimationAtlas& operator=(AnimationAtlas&&) = delete;

    Animation* getAnimation(const std::string& name);

    friend AnimationRange;
  };

  struct Vector;
  struct Color;
  class Body;

  class Renderer
  {
  public:
    static void init(Ref<Shader>& shader, int width, int height, GLuint& buffer);
    static void shutdown();

    static void beginBatch();
    static void endBatch();

    static void onscreen();
    static void offscreen();

    static void draw(const Vector* vertices, int numVertices, const uint32_t* triangles, const Color& color);
    static void draw(const Vector& a, const Vector& b, const Vector& c, const Vector& d, uint32_t textureID, float tilingFactor, const Color& tint);
    static void draw(const Vector& a, const Vector& b, const Vector& c, const Vector& d, uint32_t textureID, const glm::vec4& texturePosition, const Color& tint);
    static void draw(const Vector& a, const Vector& b, const Vector& c, const Vector& d, TextureAtlas* textureAtlas, float x, float y, float w, float h, const Color& tint);
  };

  GLuint loadTexture(const std::string& filepath, bool nearest, bool repeat);
  GLuint createFrameBuffer(int width, int height, GLuint& buffer);

};