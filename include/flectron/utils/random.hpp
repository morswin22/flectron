#pragma once
#include <random>

namespace flectron
{

  static std::random_device randomDevice;
  static std::mt19937 randomEngine(randomDevice());
  static std::mt19937_64 randomEngine64(randomDevice());

  static std::uniform_int_distribution<> randomBoolDistribution(0, 1);
  bool randomBool();

  static std::uniform_int_distribution<uint64_t> randomUUIDDistribution;
  uint64_t randomUUID();

  // Deprecated
  int randomInt(int min, int max);  
  float randomFloat(float min, float max);
  double randomDouble(double min, double max);

}
