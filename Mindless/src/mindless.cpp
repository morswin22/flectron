#include <iostream>
#include <list>
#include <memory>
#include <sstream>

#include <MindlessEngine/MindlessEngine.hpp>

using namespace MindlessEngine;

class Mindless : public Game
{
private:
  Ref<Shader> batchShader;
  Ref<TextureAtlas> textureAtlas;
  Ref<FontAtlas> fontAtlas;
  Ref<Body> platform;
  Ref<LightScene> lightScene;
  Ref<TimeScene> timeScene;
  Color nightColor;

public:
  Mindless()
  : Game(),
    batchShader(createRef<Shader>("shaders/batch.vert", "shaders/batch.frag")), 
    textureAtlas(createRef<TextureAtlas>("assets/atlas.png", 9, 2, true)), 
    fontAtlas(createRef<FontAtlas>("assets/font.png", 13, 7, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()[]{}-=+/\\|?.,<>~:; ")),
    platform(nullptr),
    lightScene(createRef<LightScene>("shaders/light.comp", window.width, window.height)),
    timeScene(createRef<TimeScene>(8.0f/24.0f, 5.0f/24.0f, 18.0f/24.0f, 3.0f/24.0f, 0.03f, 0.0f, 0.95f)),
    nightColor(0.14f, 0.22f, 0.25f, 1.0f)
  {
    window.setTitle("Mindless");
    window.setBackground(Colors::darkGray());
    window.camera.setScale(0.06f);

    int samplers[window.maxTextureSlots];
    for (int i = 0; i < window.maxTextureSlots; i++)
      samplers[i] = i;

    batchShader->bind();
    batchShader->setUniform1iv("uTextures", samplers, window.maxTextureSlots);

    glm::vec4 constraints = window.camera.getConstraints();
    float padding = (constraints.t - constraints.s) * 0.10f;
    platform = createBoxBody(constraints.t - constraints.s - padding * 2.0f, 3.0f, { 0.0f, -10.0f }, 1.0f, 0.5f, true);
    platform->texture(textureAtlas, 0.0f, 0.0f, 9.0f, 1.0f);
    world.addBody(platform);
  }

  void update() override
  {
    // TODO change for Mouse::isClicked()
    if (Mouse::isPressed(Buttons::LEFT))
    {
      float radius = randomFloat(0.75f, 1.25f);

      Ref<Body> body = createCircleBody(radius, mouseWorldPosition, 2.0f, 0.6f, false);
      body->fill(Colors::random());
      body->stroke(Colors::white());
      world.addBody(body);
    }

    if (Mouse::isPressed(Buttons::RIGHT))
    {
      float width = randomFloat(1.0f, 2.0f);
      float height = randomFloat(1.0f, 2.0f);

      Ref<Body> body = createBoxBody(width, height, mouseWorldPosition, 2.0f, 0.6f, false);
      body->texture(textureAtlas, (float)randomInt(0, 5), 1.0f, 1.0f, 1.0f);
      world.addBody(body);
    }

    if (Keyboard::isPressed(Keys::Q))
      platform->rotate(elapsedTime * (float)M_PI * 0.5f);
    if (Keyboard::isPressed(Keys::E))
      platform->rotate(-elapsedTime * (float)M_PI * 0.5f);

    world.update(elapsedTime);
    removeOffscreen();

    timeScene->update(elapsedTime);
  }

  void render() override
  {
    lightScene->addLight({ window.width * 0.5f, window.height * 0.5f }, sinf(timeScene->getTime() * 100.0f) * 10.0f + 95.0f);
    lightScene->addLight({ mousePosition.x, mousePosition.y }, 150.0f);
    lightScene->calculate({ nightColor.r, nightColor.g, nightColor.b, timeScene->getDarkness() });

    window.clear();

    batchShader->bind();
    batchShader->setUniformMat4f("uViewProjection", window.camera.getViewProjectionMatrix());

    for (int i = 0; i < world.getBodyCount(); i++)
      window.draw(world.getBody(i));

    window.draw(lightScene);

    std::ostringstream text;
    text << "Elapsed time: " << (int)(elapsedTime * 1000.0f) << "ms";
    text << "\nNumber of bodies: " << world.getBodyCount();
    text << "\n" << timeScene->getDay() << "d " << floorf(timeScene->getTime() * 24.0f) << "h (" << floorf(timeScene->getDarkness() * 100.0f) / 100.f << ")";

    if (Keyboard::isPressed(Keys::H))
      text << "\n\nPress Q/E to rotate\nPress LEFT/RIGHT to add circles/boxes";

    const float scale = window.camera.getScale();
    const glm::vec4& constraints = window.camera.getConstraints();
    window.draw(fontAtlas, { constraints.s, constraints.q }, text.str(), 175.0f * scale, Colors::white());
    
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