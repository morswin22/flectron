#include <iostream>
#include <list>
#include <memory>

#include <MindlessEngine/MindlessEngine.hpp>

using namespace MindlessEngine;

// #define RENDER_BODIES_WITH_STROKE

class Mindless : public Game
{
private:
  std::unique_ptr<Shader> basicShader;

public:
  Mindless()
   : Game(), basicShader(nullptr)
  {
    window.setTitle("Mindless");
    window.setScale(15.0f);
    window.setBackground(Colors::darkGray());

#ifdef RENDER_BODIES_WITH_STROKE
    World::numCircleVerticies = 12;
#endif

    basicShader = std::make_unique<Shader>("../MindlessEngine/shaders/basic.vert", "../MindlessEngine/shaders/basic.frag");
    
    float left, top, right, bottom;
    window.getCameraConstrains(&left, &top, &right, &bottom);

    float padding = (right - left) * 0.10f;

    world.addBody(createBoxBody(right - left - padding * 2.0f, 3.0f, { 0.0f, -10.0f }, 1.0f, 0.5f, true));
    world.getBody(0).color = Colors::darkGreen();
  }

  void update(float elapsedTime) override
  {
    std::cout << "Elapsed time: " << elapsedTime * 1000.0f << "ms" << std::endl;

    // TODO change for Mouse::isClicked()
    if (Mouse::isPressed(Buttons::LEFT))
    {
      float radius = randomFloat(0.75f, 1.25f);

      world.addBody(createCircleBody(radius, mouseWorldPosition, 2.0f, 0.6f, false));
      world.getBody(world.getBodyCount() - 1).color = Colors::random();
    }

    if (Mouse::isPressed(Buttons::RIGHT))
    {
      float width = randomFloat(1.0f, 2.0f);
      float height = randomFloat(1.0f, 2.0f);

      world.addBody(createBoxBody(width, height, mouseWorldPosition, 2.0f, 0.6f, false));
      world.getBody(world.getBodyCount() - 1).color = Colors::random();
    }

#if true
    if (world.getBodyCount() > 0)
    {
      Body& myBody = world.getBody(0);

#if false
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
#endif

      if (Keyboard::isPressed(Keys::Q))
        myBody.rotate(elapsedTime * (float)M_PI * 0.5f);
      if (Keyboard::isPressed(Keys::E))
        myBody.rotate(-elapsedTime * (float)M_PI * 0.5f);
    }
#endif

    world.update(elapsedTime);
    removeOffscreen();

    std::cout << "Number of bodies: " << world.getBodyCount() << "\n";
  }

  void render() override
  {
    window.clear();

    basicShader->bind();
    basicShader->setUniformMat4f("uMVP", window.getProjectionMatrix());
    
    Color strokeColor = Colors::white();
    for (int i = 0; i < world.getBodyCount(); i++)
    {
      Body& body = world.getBody(i);

#ifdef RENDER_BODIES_WITH_STROKE
      basicShader->setUniform4f("uColor", strokeColor.r, strokeColor.g, strokeColor.b, strokeColor.a);
      Vector* vertices = body.getTransformedVertices();
      int numVertices = body.getNumVertices();
      for (int j = 0; j < numVertices; j++)
      {
        window.drawLine(vertices[j], vertices[(j + 1) % numVertices]);
      }
#endif

      basicShader->setUniform4f("uColor", body.color.r, body.color.g, body.color.b, body.color.a);
      window.draw(body);
    }
  }
};

int main(void)
{
  Mindless mindless;
  mindless.run();

  return 0;
}