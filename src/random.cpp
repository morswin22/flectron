#include <flectron/random.hpp>

#include <random>

namespace flectron 
{

  bool randomBool()
  {
    return rand() % 2 == 0;
  }

  int randomInt(int min, int max)
  {
    return rand() % (max - min) + min;
  }

  float randomFloat(float min, float max)
  {
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (max - min) + min;
  }

}
