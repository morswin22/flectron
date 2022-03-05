#pragma once
#include <flectron/application/application.hpp>
#include <flectron/scene/scene.hpp>

namespace flectron
{

  class Layer
  {
  protected:
    Application& application;

  public:
    Layer(Application& application);
    virtual ~Layer();

    virtual void setup();
    virtual void update();
    virtual void cleanup();
  };

  class SceneLayer : public Layer
  {
  protected:
    Scene scene;

  public:
    SceneLayer(Application& application, size_t physicsIterations, size_t gridSize);

    virtual void update() override;
    virtual void cleanup() override;
  };

}
