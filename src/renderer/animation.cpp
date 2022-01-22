#include <flectron/renderer/animation.hpp>

#include <sstream>
#include <fstream>
#include <string>
#include <flectron/utils/random.hpp>

namespace flectron 
{

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

}
