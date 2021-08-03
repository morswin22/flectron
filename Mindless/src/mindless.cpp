#include <iostream>
#include <MindlessEngine/MindlessEngine.hpp>

using namespace MindlessEngine;

int main(void)
{

  Vector a(1.0f, 2.0f);
  Vector b(3.0f, 4.0f);

  Vector c = a + b;

  std::cout << normalize(c) << "\n";

  Window window(640, 480, "Mindless Test");
  window.mainloop();

  return 0;
}