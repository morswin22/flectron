#include <iostream>
#include <sstream>

#include <flectron.hpp>

using namespace flectron;

class Demo : public Game
{
private:
  Ref<TextureAtlas> textureAtlas;
  Ref<AnimationAtlas> animationAtlas;
  Ref<FontAtlas> fontAtlas;
  Entity platform;
  Entity player;

public:
  Demo()
  : Game(640, 480, "Flectron Demo", "shaders/batch.vert", "shaders/batch.frag"),
    textureAtlas(createRef<TextureAtlas>("assets/atlas.png", 9, 2, true)),
    animationAtlas(createRef<AnimationAtlas>("assets/greenGoblin.png", 12, 10, true, "assets/greenGoblin.txt")),
    fontAtlas(createRef<FontAtlas>("assets/font.png", 13, 7, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()[]{}-=+/\\|?.,<>~:; "))
  {
    window.setBackground(Colors::darkGray());
    window.camera.setScale(0.06f);

    scene.environment.nightColor = { 0.0f, 0.39f, 0.53f, 1.0f };
    scene.dateTime = createScope<DateTime>(8.0f/24.0f, 5.0f/24.0f, 18.0f/24.0f, 3.0f/24.0f, 1.0f/24.0f, 0.0f, 1.0f);
    scene.lightRenderer = createScope<LightRenderer>("shaders/light.vert", "shaders/light.frag");
    
    const Constraints& cc = window.camera.getConstraints();
    float padding = (cc.right - cc.left) * 0.10f;
    platform = scene.createEntity("Platform", { 0.0f, -10.0f }, 0.0f);
    platform.add<BoxComponent>(cc.right - cc.left - padding * 2.0f, 3.0f);
    platform.add<PhysicsComponent>(platform.get<VertexComponent>(), 1.0f, 0.5f, true);
    platform.add<TextureComponent>(textureAtlas, 0.0f, 0.0f, 9.0f, 1.0f);
    platform.add<PermanentComponent>();
    platform.add<ScriptComponent>([&]() {
      if (Keyboard::isPressed(Keys::Q))
        platform.get<PositionComponent>().rotate(elapsedTime * (float)M_PI * 0.5f);
      if (Keyboard::isPressed(Keys::E))
        platform.get<PositionComponent>().rotate(-elapsedTime * (float)M_PI * 0.5f);
    });

    player = scene.createEntity("Player", { 0.0f, 0.0f }, 0.0f);
    player.add<BoxComponent>(3.0f, 4.4f);
    player.add<PhysicsComponent>(player.get<VertexComponent>(), 1.0f, 0.1f, false);
    player.add<AnimationComponent>(animationAtlas, "idle");
    player.add<TextureVertexComponent>(Vector(0.0f, 0.4f), Vector(4.5f, 3.0f));
    player.add<PermanentComponent>();
    player.add<ScriptComponent>([&]() {
      if (Keyboard::isPressed(Keys::T))
        player.get<AnimationComponent>().play("taunt");
    });

    auto triangle = scene.createEntity("Triangle", { 8.0f, 0.0f }, 0.0f);
    triangle.add<PolygonComponent>(std::vector<Vector>{ { -1.0f, -1.0f }, { 1.0f, -1.0f }, { 0.0f,  1.0f } });
    triangle.add<PhysicsComponent>(triangle.get<VertexComponent>(), 1.0f, 0.1f, false);
    triangle.add<FillComponent>(Colors::lightCyan());
    triangle.add<LightComponent>(4.0f, Colors::cyan());

    scene.createScript("Spawner", [&]() {
      if (Mouse::isPressed(Buttons::LEFT))
      {
        auto entity = scene.createEntity("Circle", mouseWorldPosition, 0.0f);
        auto& cc = entity.add<CircleComponent>(randomFloat(0.75f, 1.25f));
        entity.add<PhysicsComponent>(entity.get<VertexComponent>(), 2.0f, 0.6f, false);
        entity.add<FillComponent>(Colors::random());
        entity.add<StrokeComponent>(Colors::white(), 1.0f);

        if (randomFloat(0.0f, 1.0f) < 0.1f)
          entity.add<LightComponent>(cc.radius * 3.0f, Colors::random());
      }

      if (Mouse::isPressed(Buttons::RIGHT))
      {
        auto entity = scene.createEntity("Box", mouseWorldPosition, 0.0f);
        auto& bc = entity.add<BoxComponent>(randomFloat(1.0f, 2.0f), randomFloat(1.0f, 2.0f));
        entity.add<PhysicsComponent>(entity.get<VertexComponent>(), 2.0f, 0.6f, false);
        entity.add<TextureComponent>(textureAtlas, (float)randomInt(0, 5), 1.0f, 1.0f, 1.0f);

        if (randomFloat(0.0f, 1.0f) < 0.1f)
          entity.add<LightComponent>(std::max(bc.width, bc.height) * 3.0f, Colors::random());
      }
    });

    scene.createScript("Mouse Light", [&]() {
      if (Keyboard::isPressed(Keys::L))
        scene.lightRenderer->addLight(mouseWorldPosition, 9.0f, Colors::white());
    });

    scene.createScript("Remove Offscreen", [&]() {
      scene.removeEntitiesOutside<PhysicsComponent>(window.camera.getConstraints());
    }, FLECTRON_PHYSICS);

    scene.createScript("Info Text", [&]() {
      std::ostringstream text;
      text << "Elapsed time: " << (int)(elapsedTime * 1000.0f) << "ms";
      text << " (" << (int)(physicsTime * 1000.0f) << "ms)";
      text << "\nNumber of entities: " << scene.getEntityCount<PhysicsComponent>();
      text << " (" << scene.getEntityCount("Circle") << "|" << scene.getEntityCount("Box") << ")";
      text << "\n" << scene.dateTime->getDay() << "d " << std::floorf(scene.dateTime->getTime() * 24.0f);
      text << "h (" << std::floorf(scene.dateTime->getDarkness() * 100.0f) / 100.f << ")";

      if (Keyboard::isPressed(Keys::H))
        text << "\n\nPress Q/E to rotate\nPress LEFT/RIGHT to add circles/boxes";

      const float scale = window.camera.getScale();
      const Constraints& cc = window.camera.getConstraints();
      window.draw(fontAtlas, { cc.left, cc.bottom }, text.str(), 175.0f * scale, Colors::white());
    }, FLECTRON_RENDER);

    scene.createScript("Camera Controller", [&]() {
      window.camera.handleWASD();
      window.camera.handleScroll();
    }, FLECTRON_RENDER + 1);
  }
};

int main(void)
{
  Demo test;
  test.run();

  return 0;
}
