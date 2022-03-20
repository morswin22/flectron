#include <flectron.hpp>
#include <flectron/application/entry.hpp>

#include <iostream>

using namespace flectron;

class Box : public Entity
{
private:
  double speed;
  double mass;
public:
  Box(Entity entity, double size, double mass) 
    : Entity(entity),
      speed(0.0f),
      mass(mass)
  {
    add<BoxComponent>(size, size);
    add<FillComponent>(Colors::white());
    add<StrokeComponent>(Colors::black(), 1.0f);
  }

  void reverseSpeed()
  {
    speed = -speed;
  }

  void applySpeed(double newSpeed)
  {
    speed = newSpeed;
  }

  double resolveCollision(const Box& other)
  {
    double sumM = mass + other.mass;
    return ((mass - other.mass) / sumM) * speed + ((2 * other.mass / sumM) * other.speed);
  }

  bool collide(Box& other)
  {
    Collision collision;
    return ::collide(get<PositionComponent>(), get<VertexComponent>(), other.get<PositionComponent>(), other.get<VertexComponent>(), collision);
  }

  void update()
  {
    get<PositionComponent>().move({ static_cast<float>(speed), 0.0f });
  }
};

#define STEPS_PER_FRAME 2500
#define PI_DIGITS 5

class PiDayLayer : public SceneLayer
{
private:
  Ref<FontAtlas> font;
  ::Box smallBox;
  ::Box largeBox;
  int score;

public:
  PiDayLayer(Application& application)
  : SceneLayer(application, 0u, 4u),
    font(createRef<FontAtlas>("assets/font.png", 13, 7, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()[]{}-=+/\\|?.,<>~:; ")),
    smallBox(scene.createEntity("Smaller box", {}, {}), 50.0f, 1.0),
    largeBox(scene.createEntity("Bigger box", {}, {}), 100.0f, std::pow(100.0, static_cast<double>(PI_DIGITS - 1))),
    score(0)
  {
    application.window.setBackground(Colors::darkGray());
  }

  void setup() override
  {
    smallBox.get<PositionComponent>().move({ -50.0f, -25.0f });
    largeBox.get<PositionComponent>().move({ 150.0f, 0.0f });
    largeBox.applySpeed(-1.0 / static_cast<double>(STEPS_PER_FRAME));
  }

  void update() override
  {
    for (int i = 0; i < STEPS_PER_FRAME; ++i)
    {
      if (smallBox.collide(largeBox))
      {
        double newSmallBoxSpeed = smallBox.resolveCollision(largeBox);
        double newLargeBoxSpeed = largeBox.resolveCollision(smallBox);
        smallBox.applySpeed(newSmallBoxSpeed);
        largeBox.applySpeed(newLargeBoxSpeed);
        score++;
      }

      if (smallBox.get<PositionComponent>().position.x <= -200.0f)
      {
        smallBox.reverseSpeed();
        score++;
      }

      smallBox.update();
      largeBox.update();
    }
    
    SceneLayer::update();

    Renderer::debugLine({ -225.0f, 100.0f }, { -225.0f, -50.0f });
    Renderer::debugLine({ -225.0f, -50.0f }, { 300.0f, -50.0f });

    std::string scoreString = "pi = " + std::to_string(score);
    scoreString.insert(scoreString.begin() + 6, '.');
    Renderer::text(font, { -225.0f, -50.0f }, scoreString, 200.0f);
  }
};

class PiDayApplication : public Application
{
public:
  PiDayApplication() : Application({}, { "Pi Day 2022", 600, 400, VSYNC })
  {
    pushLayer<PiDayLayer>();
  }
};

Scope<Application> flectron::createApplication(const ApplicationArguments&)
{
  return createScope<PiDayApplication>();
}
