#include <iostream>
#include <list>

#include <MindlessEngine/MindlessEngine.hpp>

using namespace MindlessEngine;

int main(void)
{

  Vector a(1.0f, 2.0f);
  Vector b(3.0f, 4.0f);

  Vector c = a + b;

  std::cout << normalize(c) << "\n";

  Window window(640, 480, "Mindless Test");

  std::list<Body> bodyList;
  int bodyCount = 10;
  float padding = 20.0f;

  for (int i = 0; i < bodyCount; i++)
  {
    BodyType type = static_cast<BodyType>(randomInt(0, 2));

    float x = randomFloat(padding, window.width - padding);
    float y = randomFloat(padding, window.height - padding);

    if (type == BodyType::Circle)
    {
      bodyList.push_back(createCircleBody(3.0f, { x, y }, 2.0f, 0.5f, false));
    }
    else if (type == BodyType::Box)
    {
      bodyList.push_back(createBoxBody(3.0f, 3.0f, { x, y }, 2.0f, 0.5f, false));
    }
    else
      throw std::invalid_argument("Invalid body type");
  }

  window.mainloop();

  return 0;
}