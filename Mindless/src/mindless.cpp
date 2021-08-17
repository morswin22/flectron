#include <iostream>
#include <list>
#include <memory>
#include <sstream>

#include <MindlessEngine/MindlessEngine.hpp>

using namespace MindlessEngine;

class Mindless : public Game
{
private:
  std::unique_ptr<Shader> batchShader;
  std::unique_ptr<TextureAtlas> textureAtlas;
  std::unique_ptr<FontAtlas> fontAtlas;

public:
  Mindless()
   : Game(), batchShader(nullptr), textureAtlas(nullptr), fontAtlas(nullptr)
  {
    window.setTitle("Mindless");
    window.setBackground(Colors::darkGray());
    window.camera.setScale(0.06f);

    batchShader = std::make_unique<Shader>("shaders/batch.vert", "shaders/batch.frag");

    int samplers[window.maxTextureSlots];
    for (int i = 0; i < window.maxTextureSlots; i++)
      samplers[i] = i;

    batchShader->bind();
    batchShader->setUniform1iv("uTextures", samplers, window.maxTextureSlots);
    
    textureAtlas = std::make_unique<TextureAtlas>("assets/atlas.png", 9, 2, true);
    fontAtlas = std::make_unique<FontAtlas>("assets/font.png", 13, 7, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()[]{}-=+/\\|?.,<>~:; ");

    glm::vec4 constraints = window.camera.getConstraints();
    float padding = (constraints.t - constraints.s) * 0.10f;
    Body platform = createBoxBody(constraints.t - constraints.s - padding * 2.0f, 3.0f, { 0.0f, -10.0f }, 1.0f, 0.5f, true);
    platform.texture(*textureAtlas.get(), 0.0f, 0.0f, 9.0f, 1.0f);
    world.addBody(platform);
  }

  void update() override
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

    if (world.getBodyCount() > 0)
    {
      Body& myBody = world.getBody(0);

      if (Keyboard::isPressed(Keys::Q))
        myBody.rotate(elapsedTime * (float)M_PI * 0.5f);
      if (Keyboard::isPressed(Keys::E))
        myBody.rotate(-elapsedTime * (float)M_PI * 0.5f);
    }

    world.update(elapsedTime);
    removeOffscreen();
  }

  void render() override
  {
    window.clear();

    batchShader->bind();
    batchShader->setUniformMat4f("uViewProjection", window.camera.getViewProjectionMatrix());

    for (int i = 0; i < world.getBodyCount(); i++)
      window.draw(world.getBody(i));

    std::ostringstream text;
    text << "Elapsed time: " << (int)(elapsedTime * 1000.0f) << "ms\n";
    text << "Number of bodies: " << world.getBodyCount();

    if (Keyboard::isPressed(Keys::H))
      text << "\n\nPress Q/E to rotate\nPress LEFT/RIGHT to add circles/boxes";

    const float scale = window.camera.getScale();
    const glm::vec4& constraints = window.camera.getConstraints();
    window.draw(*fontAtlas.get(), { constraints.s, constraints.q }, text.str(), 175.0f * scale, Colors::white());
    
    // Handle camera events
    float dx = 0.0f;
    float dy = 0.0f;

    if (Keyboard::isPressed(Keys::W))
      dy++;
    if (Keyboard::isPressed(Keys::S))
      dy--;
    if (Keyboard::isPressed(Keys::A))
      dx--;
    if (Keyboard::isPressed(Keys::D))
      dx++;

    if (dx != 0.0f || dy != 0.0f)
      window.camera.move(normalize({ dx, dy }) * scale * 8.0f);

    const float scrollY = Mouse::getScrollY();
    if (scrollY != 0.0f)
    {
      const float amount = (scrollY > 0.0f) ? 0.8f : 1.25f;
      window.camera.setScale(scale * amount);
    }
  }
};

int main(void)
{
  Mindless mindless;
  mindless.run();

  return 0;
}