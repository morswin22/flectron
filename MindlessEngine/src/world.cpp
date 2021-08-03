#include <MindlessEngine/world.hpp>

namespace MindlessEngine 
{

  float World::minBodySize = 0.01f * 0.01f;
  float World::maxBodySize = 64.0f * 64.0f;

  float World::minBodyDensity = 0.5f; // half of water density
  float World::maxBodyDensity = 21.4f; // platinum density

  int World::numCircleVerticies = 64;
  
};