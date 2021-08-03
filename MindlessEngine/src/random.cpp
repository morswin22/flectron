#include <MindlessEngine/random.hpp>

#include <random>

namespace MindlessEngine 
{

  int randomInt(int min, int max)
  {
    return rand() % (max - min) + min;
  }

  float randomFloat(float min, float max)
  {
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (max - min) + min;
  }

};