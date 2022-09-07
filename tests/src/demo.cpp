#include <flectron.hpp>
#include <flectron/application/entry.hpp>
#include <sstream>

FLECTRON_EMBED(ATLAS_PNG);
FLECTRON_EMBED(FONT_PNG);
FLECTRON_EMBED(GREENGOBLIN_PNG);
FLECTRON_EMBED(GREENGOBLIN_TXT);

using namespace flectron;

class DemoLayer : public SceneLayer
{
private:
  Stopwatch physicsTimer;
  Image atlasImage, fontImage, greenGoblinImage;
  Text greenGoblinText;
  Ref<TextureAtlas> textureAtlas;
  Ref<AnimationAtlas> animationAtlas;
  Ref<FontAtlas> fontAtlas;
  Entity platform;
  Entity player;
  SceneAsset sceneFile;

public:
  DemoLayer(Application& application)
  : SceneLayer(application, 4u, 4u), physicsTimer(),
    atlasImage(Image::fromEmbed(ATLAS_PNG())),
    fontImage(Image::fromEmbed(FONT_PNG())),
    greenGoblinImage(Image::fromEmbed(GREENGOBLIN_PNG())),
    greenGoblinText(Text::fromEmbed(GREENGOBLIN_TXT())),
    textureAtlas(nullptr), animationAtlas(nullptr), fontAtlas(nullptr),
    sceneFile(SceneAsset::fromFile("scene.demo"))
  {}

  void setup() override
  {
    application.window.setBackground(Colors::darkGray());
    application.window.camera.setScale(0.06f);

    scene.environment.nightColor = { 0.0f, 0.39f, 0.53f, 1.0f };
    scene.dateTime = createScope<DateTime>(8.0f/24.0f, 5.0f/24.0f, 18.0f/24.0f, 3.0f/24.0f, 1.0f/24.0f, 0.0f, 1.0f);
    scene.lightRenderer = createScope<LightRenderer>();

    atlasImage.load();
    atlasImage.loadGPU();
    atlasImage.unload();

    fontImage.load();
    fontImage.loadGPU();
    fontImage.unload();

    greenGoblinImage.load();
    greenGoblinImage.loadGPU();
    greenGoblinImage.unload();

    greenGoblinText.load();

    textureAtlas = createRef<TextureAtlas>(atlasImage, 9, 2);
    animationAtlas = createRef<AnimationAtlas>(greenGoblinImage, 12, 10, greenGoblinText);
    fontAtlas = createRef<FontAtlas>(fontImage, 13, 7, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()[]{}-=+/\\|?.,<>~:; ");

    const Constraints& cc = application.window.camera.getConstraints();
    float padding = (cc.right - cc.left) * 0.10f;
    platform = scene.createEntity("Platform", { 0.0f, -10.0f }, 0.0f);
    platform.add<UUIDComponent>();
    platform.add<BoxComponent>(cc.right - cc.left - padding * 2.0f, 3.0f);
    platform.add<PhysicsComponent>(1.0f, 0.5f, true);
    platform.add<TextureComponent>(textureAtlas, 0.0f, 0.0f, 9.0f, 1.0f);
    platform.add<ScriptComponent>([&]() {
      if (Keyboard::isPressed(Key::Q))
        platform.get<PositionComponent>().rotate(application.elapsedTime * (float)M_PI * 0.5f);
      if (Keyboard::isPressed(Key::E))
        platform.get<PositionComponent>().rotate(-application.elapsedTime * (float)M_PI * 0.5f);
    });

    player = scene.createEntity("Player", { 0.0f, 0.0f }, 0.0f);
    player.add<UUIDComponent>();
    player.add<BoxComponent>(3.0f, 4.4f);
    player.add<PhysicsComponent>(1.0f, 0.1f, false);
    player.add<AnimationComponent>(animationAtlas, "idle");
    player.add<TextureVertexComponent>(Vector(0.0f, 0.4f), Vector(9.0f, 6.0f));
    player.add<ScriptComponent>([&]() {
      if (Keyboard::isPressed(Key::T))
        player.get<AnimationComponent>().play("taunt");
    });

    auto triangle = scene.createEntity("Triangle", { 8.0f, 0.0f }, 0.0f);
    triangle.add<UUIDComponent>();
    triangle.add<PolygonComponent>(std::vector<Vector>{ { -1.0f, -1.0f }, { 1.0f, -1.0f }, { 0.0f,  1.0f } });
    triangle.add<PhysicsComponent>(1.0f, 0.1f, false);
    triangle.add<FillComponent>(Colors::lightCyan());
    triangle.add<LightComponent>(4.0f, Colors::cyan());

    scene.createScript("Spawner", [&]() {
      if (Mouse::isClicked(Button::LEFT) || (Keyboard::isPressed(Key::LEFT_CONTROL) && Mouse::isPressed(Button::LEFT)))
      {
        auto entity = scene.createEntity("Circle", application.mouseWorldPosition, 0.0f);
        auto& cc = entity.add<CircleComponent>(randomFloat(0.75f, 1.25f));
        entity.add<PhysicsComponent>(2.0f, 0.6f, false);
        entity.add<FillComponent>(Colors::random());
        entity.add<StrokeComponent>(Colors::white(), 0.05f);
        entity.add<TemporaryComponent>();
        entity.add<UUIDComponent>();

        if (randomFloat(0.0f, 1.0f) < 0.1f)
          entity.add<LightComponent>(cc.radius * 3.0f, Colors::random());
      }

      if (Mouse::isClicked(Button::RIGHT) || (Keyboard::isPressed(Key::LEFT_CONTROL) && Mouse::isPressed(Button::RIGHT)))
      {
        auto entity = scene.createEntity("Box", application.mouseWorldPosition, 0.0f);
        auto& bc = entity.add<BoxComponent>(randomFloat(1.0f, 2.0f), randomFloat(1.0f, 2.0f));
        entity.add<PhysicsComponent>(2.0f, 0.6f, false);
        entity.add<TextureComponent>(textureAtlas, (float)randomInt(0, 5), 1.0f, 1.0f, 1.0f);
        entity.add<TemporaryComponent>();
        entity.add<UUIDComponent>();

        if (randomFloat(0.0f, 1.0f) < 0.1f)
          entity.add<LightComponent>(std::max(bc.width, bc.height) * 3.0f, Colors::random());
      }

      FLECTRON_ASSERT(!Keyboard::isClicked(Key::P), "P is the forbidden key!");
    });

    scene.createScript("Mouse Light", [&]() {
      if (Keyboard::isPressed(Key::L))
        scene.lightRenderer->addLight(application.mouseWorldPosition, 9.0f, Colors::white());
    });

    scene.createScript("Remove Offscreen", [&]() {
      scene.removeEntitiesOutside<TemporaryComponent>(application.window.camera.getConstraints());
    }, FLECTRON_PHYSICS);

    scene.createScript("Physics Timer Start", [&]() {
      physicsTimer.start();
    }, FLECTRON_PHYSICS - 1);

    scene.createScript("Physics Timer Stop", [&]() {
      physicsTimer.stop();
    }, FLECTRON_PHYSICS);

    scene.createScript("Info Text", [&]() {
      const auto& stats = Renderer::statistics();

      std::ostringstream text;
      text << "Elapsed time: " << (int)(application.elapsedTime * 1000.0f) << "ms";
      text << " (" << (int)(physicsTimer.getElapsedTime() * 1000.0f) << "ms)";
      text << "\nRenderer stats: ";
      text << std::floor(stats.texturePercentage() * 100.0f) << "%|";
      text << std::floor(stats.circlePercentage() * 100.0f) << "%|";
      text << std::floor(stats.linePercentage() * 100.0f) << "% (";
      text << stats.totalDrawCalls() << ")";
      text << "\nSpawned entities: " << scene.getEntityCount<TemporaryComponent>();
      text << " (" << scene.getEntityCount("Circle") << "|" << scene.getEntityCount("Box") << ")";
      text << "\n" << scene.dateTime->getDay() << "d " << std::floor(scene.dateTime->getTime() * 24.0f);
      text << "h (" << std::floor(scene.dateTime->getDarkness() * 100.0f) / 100.f << ")";

      if (Keyboard::isPressed(Key::H))
        text << "\n\nPress Q/E to rotate\nPress LEFT/RIGHT to add circles/boxes";

      const float scale = application.window.camera.getScale();
      const Constraints& cc = application.window.camera.getConstraints();
      Renderer::text(fontAtlas, { cc.left, cc.bottom }, text.str(), 175.0f * scale);
    }, FLECTRON_RENDER);

    scene.createScript("Camera Controller", [&]() {
      application.window.camera.handleWASD();
      application.window.camera.handleScroll();
    }, FLECTRON_RENDER + 1);

    scene.createScript("Scene Saver", [&]() {
      if (Keyboard::isPressed(Key::LEFT_CONTROL) && Keyboard::isClicked(Key::S))
        scene.serialize(sceneFile);
      if (Keyboard::isPressed(Key::LEFT_CONTROL) && Keyboard::isClicked(Key::X))
      {
        // TODO file might not exist
        sceneFile.load();
        {
          auto view = scene.registry.view<TagComponent>();
          for (auto entity : view)
            if (view.get<TagComponent>(entity).tag == "Circle" || view.get<TagComponent>(entity).tag == "Box")
              scene.registry.destroy(entity);
        }
        scene.deserialize(sceneFile, [&](Entity entity) {
          const auto& tag = entity.get<TagComponent>().tag;
          if (tag == "Player")
          {
            player.destroy();
            player = entity;
            player.add<AnimationComponent>().animationAtlas = animationAtlas;
            player.add<ScriptComponent>([&]() {
            if (Keyboard::isPressed(Key::T))
              player.get<AnimationComponent>().play("taunt");
            });
          }
          else if (tag == "Platform")
          {
            platform.destroy();
            platform = entity;
            platform.add<TextureComponent>(textureAtlas, 0.0f, 0.0f, 9.0f, 1.0f);
            platform.add<ScriptComponent>([&]() {
              if (Keyboard::isPressed(Key::Q))
                platform.get<PositionComponent>().rotate(application.elapsedTime * (float)M_PI * 0.5f);
              if (Keyboard::isPressed(Key::E))
                platform.get<PositionComponent>().rotate(-application.elapsedTime * (float)M_PI * 0.5f);
            });
          }
          else if (tag == "Triangle")
          {
            auto view = scene.registry.view<TagComponent>();
            for (auto other : view)
              if (view.get<TagComponent>(other).tag == "Triangle" && entity != other)
                scene.registry.destroy(other);
          }
          else if (tag == "Box")
          {
            entity.add<TextureComponent>(textureAtlas->image->getGPU());
          }
        });
      }
    });
  }

  void cleanup() override
  {
    atlasImage.unloadGPU();
    fontImage.unloadGPU();
    greenGoblinImage.unloadGPU();

    greenGoblinText.unload();
  }
};

class DemoApplication : public Application
{
public:
  DemoApplication() : Application({}, { "Flectron Demo", 640, 480, VSYNC })
  {
    pushLayer<DemoLayer>();
  }
};

Scope<Application> flectron::createApplication(const ApplicationArguments&)
{
  return createScope<DemoApplication>();
}
