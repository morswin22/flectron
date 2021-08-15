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
  std::unique_ptr<TextureAtlas> textureAtlas;

public:
  Mindless()
   : Game(), batchShader(nullptr), textureAtlas(nullptr)
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
    
    textureAtlas = std::make_unique<TextureAtlas>("assets/atlas.png", 9, 2, true);

    float left, top, right, bottom;
    window.getCameraConstrains(&left, &top, &right, &bottom);

    float padding = (right - left) * 0.10f;

    Body platform = createBoxBody(right - left - padding * 2.0f, 3.0f, { 0.0f, -10.0f }, 1.0f, 0.5f, true);
    platform.texture(*textureAtlas.get(), 0.0f, 0.0f, 9.0f, 1.0f);
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
      body.texture(*textureAtlas.get(), (float)randomInt(0, 5), 1.0f, 1.0f, 1.0f);
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

    for (int i = 0; i < world.getBodyCount(); i++)
      window.draw(world.getBody(i));
  }
};

int main(void)
{
  Mindless mindless;
  mindless.run();

  return 0;
}