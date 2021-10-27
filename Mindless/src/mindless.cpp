#include <iostream>
#include <list>
#include <memory>
#include <sstream>

#include <MindlessEngine/MindlessEngine.hpp>

using namespace MindlessEngine;

class Mindless : public Game
{
private:
  Ref<TextureAtlas> textureAtlas;
  Ref<FontAtlas> fontAtlas;
  Ref<Body> platform;
  Ref<LightScene> lightScene;
  Ref<TimeScene> timeScene;
  Color nightColor;
  Stopwatch physicsTimer;
  Ref<Body> player;
  Ref<AnimationAtlas> animationAtlas;

public:
  Mindless()
  : Game(640, 480, "Mindless", "shaders/batch.vert", "shaders/batch.frag"),
    textureAtlas(createRef<TextureAtlas>("assets/atlas.png", 9, 2, true)), 
    fontAtlas(createRef<FontAtlas>("assets/font.png", 13, 7, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()[]{}-=+/\\|?.,<>~:; ")),
    platform(nullptr),
    lightScene(createRef<LightScene>("shaders/light.vert", "shaders/light.frag")),
    timeScene(createRef<TimeScene>(8.0f/24.0f, 5.0f/24.0f, 18.0f/24.0f, 3.0f/24.0f, 1.0f/24.0f, 0.0f, 1.0f)),
    nightColor(0.0f, 0.39f, 0.53, 1.0f),
    player(nullptr),
    animationAtlas(createRef<AnimationAtlas>("assets/greenGoblin.png", 12, 10, true, "assets/greenGoblin.txt"))
  {
    window.setBackground(Colors::darkGray());
    window.camera.setScale(0.06f);

    const Constraints& cc = window.camera.getConstraints();
    float padding = (cc.right - cc.left) * 0.10f;
    platform = createBoxBody(cc.right - cc.left - padding * 2.0f, 3.0f, { 0.0f, -10.0f }, 1.0f, 0.5f, true);
    platform->texture(textureAtlas, 0.0f, 0.0f, 9.0f, 1.0f);
    world.addBody(platform);

    player = createBoxBody(3.0f, 4.4f, { 0.0f, 0.0f }, 1.0f, 0.1f, false);
    player->animation(animationAtlas, "idle");
    player->textureBounds({ 0.0f, 0.4f }, { 4.5f, 3.0f });
    world.addBody(player);
  }

  void update() override
  {
    if (Keyboard::isPressed(Keys::T))
      player->animation("taunt");

    // TODO change for Mouse::isClicked()
    if (Mouse::isPressed(Buttons::LEFT))
    {
      float radius = randomFloat(0.75f, 1.25f);

      Ref<Body> body = createCircleBody(radius, mouseWorldPosition, 2.0f, 0.6f, false);
      body->fill(Colors::random());
      body->stroke(Colors::white());

      if (randomFloat(0.0f, 1.0f) < 0.1f)
        body->light(radius * 3.0f, Colors::random());

      world.addBody(body);
    }

    if (Mouse::isPressed(Buttons::RIGHT))
    {
      float width = randomFloat(1.0f, 2.0f);
      float height = randomFloat(1.0f, 2.0f);

      Ref<Body> body = createBoxBody(width, height, mouseWorldPosition, 2.0f, 0.6f, false);
      body->texture(textureAtlas, (float)randomInt(0, 5), 1.0f, 1.0f, 1.0f);

      if (randomFloat(0.0f, 1.0f) < 0.1f)
        body->light(std::max(width, height) * 3.0f, Colors::random());

      world.addBody(body);
    }

    if (Keyboard::isPressed(Keys::Q))
      platform->rotate(elapsedTime * (float)M_PI * 0.5f);
    if (Keyboard::isPressed(Keys::E))
      platform->rotate(-elapsedTime * (float)M_PI * 0.5f);

    physicsTimer.start();
    world.update(elapsedTime, 4);
    physicsTimer.stop();
    removeOffscreen();

    timeScene->update(elapsedTime);
  }

  void render() override
  {
    Renderer::offscreen();
    window.clear();

    for (int i = 0; i < world.getBodyCount(); i++)
    {
      Ref<Body>& body = world.getBody(i);
      window.draw(body);
      if (body->isLit)
        lightScene->addLight(body);
    }

    if (Keyboard::isPressed(Keys::L))
      lightScene->addLight(mouseWorldPosition, 9.0f, Colors::white());

    window.draw(lightScene, nightColor, timeScene->getDarkness());

    std::ostringstream text;
    text << "Elapsed time: " << (int)(elapsedTime * 1000.0f) << "ms";
    text << " (" << (int)(physicsTimer.getElapsedTime() * 1000.0f) << "ms)";
    text << "\nNumber of bodies: " << world.getBodyCount();
    text << "\n" << timeScene->getDay() << "d " << floorf(timeScene->getTime() * 24.0f) << "h (" << floorf(timeScene->getDarkness() * 100.0f) / 100.f << ")";

    if (Keyboard::isPressed(Keys::H))
      text << "\n\nPress Q/E to rotate\nPress LEFT/RIGHT to add circles/boxes";

    const float scale = window.camera.getScale();
    const Constraints& cc = window.camera.getConstraints();
    window.draw(fontAtlas, { cc.left, cc.bottom }, text.str(), 175.0f * scale, Colors::white());
    
    window.camera.handleWASD();
    window.camera.handleScroll();
  }
};

int main(void)
{
  Mindless mindless;
  mindless.run();

  return 0;
}