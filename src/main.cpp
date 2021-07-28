#include <iostream>

#include "Mindless/common.hpp"

using namespace Mindless;

void Mindless::draw() 
{
  std::cout << mousePosition.x << ", " << mousePosition.y << std::endl;
}

int main(void) 
{
  if (init())
  {
    setTitle("Mindless");
    loop();
  }
  destroy();
}
