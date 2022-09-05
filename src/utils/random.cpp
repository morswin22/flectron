#include <flectron/utils/random.hpp>

namespace flectron 
{

  bool randomBool()
  {
    return randomBoolDistribution(randomEngine);
  }

  uint64_t randomUUID()
  {
    return randomUUIDDistribution(randomEngine64);
  }

  // Deprecated 
  
  int randomInt(int min, int max)
  {
    std::uniform_int_distribution<> distribution(min, max);
    return distribution(randomEngine);
  }

  float randomFloat(float min, float max)
  {
    std::uniform_real_distribution<float> distribution(min, max);
    return distribution(randomEngine);
  }

  double randomDouble(double min, double max)
  {
    std::uniform_real_distribution<double> distribution(min, max);
    return distribution(randomEngine);
  }

}
