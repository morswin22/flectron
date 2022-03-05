#include <flectron/application/layer.hpp>

namespace flectron
{

  Layer::Layer(Application &application)
    : application(application)
  {
  }

  Layer::~Layer()
  {
  }

  void Layer::setup()
  {
  }

  void Layer::update()
  {
  }

  void Layer::cleanup()
  {
  }

  SceneLayer::SceneLayer(Application &application, size_t physicsIterations, size_t gridSize)
    : Layer(application), scene(physicsIterations, gridSize)
  {
  }

  void SceneLayer::update()
  {
    scene.update(application);
  }

  void SceneLayer::cleanup()
  {
    scene.clear();
  }

}
