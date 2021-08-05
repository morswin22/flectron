#include <iostream>
#include <list>
#include <memory>

#include <MindlessEngine/MindlessEngine.hpp>

using namespace MindlessEngine;

class Mindless : public Game
{
private:
  std::unique_ptr<Shader> basicShader;
  std::list<Color> colorList;

public:
  Mindless()
   : Game(), basicShader(nullptr), colorList()
  {
    window.setTitle("Mindless");
    window.setScale(15.0f);
    window.setBackground(Colors::darkGray());

    basicShader = std::make_unique<Shader>("../MindlessEngine/shaders/basic.vert", "../MindlessEngine/shaders/basic.frag");
    
    float left, top, right, bottom;
    window.getCameraConstrains(&left, &top, &right, &bottom);

    float padding = (right - left) * 0.05f;

    for (int i = 0; i < 30; i++)
    {
      BodyType type = static_cast<BodyType>(randomInt(0, 2));
      if (i == 0)
        type = BodyType::Box;

      float x = randomFloat(left + padding, right - padding);
      float y = randomFloat(bottom + padding, top - padding);

      bool isStatic = randomBool() && i != 0;

      colorList.push_back(isStatic ? Colors::black() : Colors::random());

      if (type == BodyType::Circle)
      {
        world.addBody(createCircleBody(1.0f, { x, y }, 2.0f, 0.5f, isStatic));
      }
      else if (type == BodyType::Box)
      {
        world.addBody(createBoxBody(1.77f, 1.77f, { x, y }, 2.0f, 0.5f, isStatic));
      }
      else
        throw std::invalid_argument("Invalid body type");
    }
  }

  void update(float elapsedTime) override
  {
    std::cout << "Elapsed time: " << elapsedTime * 1000.0f << "ms" << std::endl;

    if (world.getBodyCount() > 0)
    {
      Body& myBody = world.getBody(0);

      float dx = 0.0f;
      float dy = 0.0f;
      float forceMagnitude = 48.0f;

      if (Keyboard::isPressed(Keys::W))
        dy++;
      if (Keyboard::isPressed(Keys::S))
        dy--;
      if (Keyboard::isPressed(Keys::A))
        dx--;
      if (Keyboard::isPressed(Keys::D))
        dx++;

      if (dx != 0.0f || dy != 0.0f)
      {
        Vector forceDirection = normalize({ dx, dy });
        Vector force = forceDirection * forceMagnitude;
        myBody.addForce(force);
      }

      if (Keyboard::isPressed(Keys::Q))
        myBody.rotate(elapsedTime * (float)M_PI * 0.5f);
      if (Keyboard::isPressed(Keys::E))
        myBody.rotate(-elapsedTime * (float)M_PI * 0.5f);
    }

    world.update(elapsedTime);
    wrapScreen();
  }

  void render() override
  {
    basicShader->bind();
    basicShader->setUniformMat4f("uMVP", window.getProjectionMatrix());
    
    auto color = colorList.begin();
    for (int i = 0; i < world.getBodyCount(); i++)
    {
      Body& body = world.getBody(i);

      basicShader->setUniform4f("uColor", color->r, color->g, color->b, color->a);
      draw(body);

      std::advance(color, 1);
    }
  }
};

int main(void)
{
  Mindless mindless;
  mindless.run();

  return 0;
}