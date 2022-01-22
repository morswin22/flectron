#pragma once

#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>
#include <string>
#include <unordered_map>

#include <flectron/utils/memory.hpp>
#include <flectron/renderer/texture.hpp>

namespace flectron
{
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

}
