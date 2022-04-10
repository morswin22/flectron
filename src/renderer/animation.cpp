#include <flectron/renderer/animation.hpp>

#include <sstream>
#include <fstream>
#include <string>
#include <flectron/utils/random.hpp>
#include <flectron/assert/assert.hpp>

namespace flectron 
{

  AnimationRange::AnimationRange(float x, float y, float width, float height, float unit, float duration, Animation* animation)
    : x(x), y(y), width(width), height(height), unit(unit), total(static_cast<size_t>(width * height)), duration(duration), animation(animation)
  {}

  glm::vec4* AnimationRange::getNext(AnimationState& state)
  {
    FLECTRON_ASSERT(
      state.currentRange <= animation->atlas->frames[state.currentName].size() && 
      state.currentFrame <= animation->atlas->frames[state.currentName][state.currentRange].size(), 
      "Animation out of range");
    return animation->atlas->frames[state.currentName][state.currentRange][state.currentFrame].get();
  }

  void AnimationRange::update(AnimationState& state, float elapsedTime)
  {
    state.elapsedTime += elapsedTime;

    if (state.elapsedTime >= duration)
    {
      state.elapsedTime = 0;
      state.currentFrame++;
    }

    if (state.currentFrame == total)
    {
      state.currentFrame = 0;
      state.currentRange++;
    }
  }

  Animation::Animation(const std::string& name, AnimationAtlas* atlas)
    : ranges(), possibleFutureAnimations(), atlas(atlas), name(name)
  {}

  void Animation::addRange(float x, float y, float width, float height, float unit, float duration)
  {
    ranges.push_back(createRef<AnimationRange>(x, y, width, height, unit, duration, this));
  }

  glm::vec4* Animation::getNext(AnimationState& state)
  {
    if (ranges.size() == 0)
      return nullptr;

    return ranges[state.currentRange]->getNext(state);
  }

  void Animation::update(AnimationState& state, float elapsedTime)
  {
    if (ranges.size() == 0)
      return;
    
    ranges[state.currentRange]->update(state, elapsedTime);

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
  }

  AnimationAtlas::AnimationAtlas(const std::string& filepath, int columns, int rows, bool nearest, const std::string& descriptionFilepath) 
    : TextureAtlas(filepath, columns, rows, nearest), animations(), frames()
  {
    std::ifstream file(descriptionFilepath);
    std::string line;
    std::string name;
    std::string position;
    std::string size;
    std::string rangeDuration;
    std::string sx, sy, sw, sh;
    float x, y, w, h, duration;

    std::getline(file, line);
    FLECTRON_ASSERT(line == "RANGES", "Invalid animation description file");

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
      std::getline(ss, rangeDuration, ' ');
      duration = std::stof(rangeDuration);

      if (animations.count(name) == 0)
        animations[name] = createRef<Animation>(name, this);
      
      animations[name]->addRange(x, y, w, h, 1.0f, duration / (w * h)); // TODO use duration or interval?

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

      FLECTRON_ASSERT(animations.count(nameA) > 0 && animations.count(nameB) > 0, "Invalid animation description file");

      animations[nameA]->possibleFutureAnimations[nameB] = weight;
    }
  }

  Animation* AnimationAtlas::getAnimation(const std::string& name)
  {
    FLECTRON_ASSERT(animations.count(name) > 0, "Animation not found");
    
    return animations[name].get();
  }

}
