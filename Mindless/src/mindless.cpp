#include <iostream>
#include <list>
#include <memory>
#include <sstream>

#include <MindlessEngine/MindlessEngine.hpp>

using namespace MindlessEngine;

#define SHOW_DEBUG_INFO

class Mindless : public Game
{
private:
  std::unique_ptr<Shader> batchShader;
  GLuint platformTexture;

public:
  Mindless()
   : Game(), batchShader(nullptr)
  {
    window.setTitle("Mindless");
    window.setScale(15.0f);
    window.setBackground(Colors::darkGray());

    batchShader = std::make_unique<Shader>("shaders/batch.vert", "shaders/batch.frag");

    int samplers[window.maxTextureSlots];
    for (int i = 0; i < window.maxTextureSlots; i++)
      samplers[i] = i;

    batchShader->bind();
    batchShader->setUniform1iv("uTextures", samplers, window.maxTextureSlots);
    
    platformTexture = loadTexture("assets/platform.png");

    float left, top, right, bottom;
    window.getCameraConstrains(&left, &top, &right, &bottom);

    float padding = (right - left) * 0.10f;

    Body platform = createBoxBody(right - left - padding * 2.0f, 3.0f, { 0.0f, -10.0f }, 1.0f, 0.5f, true);
    platform.fill(Colors::darkGreen());
    platform.stroke(Colors::white());
    world.addBody(platform);
  }

  void update(float elapsedTime) override
  {
    // TODO change for Mouse::isClicked()
    if (Mouse::isPressed(Buttons::LEFT))
    {
      float radius = randomFloat(0.75f, 1.25f);

      Body body = createCircleBody(radius, mouseWorldPosition, 2.0f, 0.6f, false);
      body.fill(Colors::random());
      body.stroke(Colors::white());
      world.addBody(body);
    }

    if (Mouse::isPressed(Buttons::RIGHT))
    {
      float width = randomFloat(1.0f, 2.0f);
      float height = randomFloat(1.0f, 2.0f);

      Body body = createBoxBody(width, height, mouseWorldPosition, 2.0f, 0.6f, false);
      body.fill(Colors::random());
      body.stroke(Colors::white());
      world.addBody(body);
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

#ifdef SHOW_DEBUG_INFO
    std::ostringstream title;
    title << "Mindless (Elapsed time: " << (int)(elapsedTime * 1000.0f) << "ms, Number of bodies: " << world.getBodyCount() << ")";
    window.setTitle(title.str());
#endif
  }

  void render() override
  {
    window.clear();

    batchShader->bind();
    batchShader->setUniformMat4f("uMVP", window.getProjectionMatrix());
    
    Body& platform = world.getBody(0);
    Vector* vertices = platform.getTransformedVertices();
    Renderer::draw(vertices[0], vertices[1], vertices[2], vertices[3], platformTexture, 1.0f);

    for (int i = 1; i < world.getBodyCount(); i++)
      window.draw(world.getBody(i));
  }
};

int main(void)
{
  Mindless mindless;
  mindless.run();

  return 0;
}