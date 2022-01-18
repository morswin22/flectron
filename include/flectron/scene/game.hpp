#pragma once

#include <flectron/scene/window.hpp>
#include <flectron/scene/scene.hpp>

namespace flectron
{
  
  class Game
  {
  protected:
    Window window;
    Vector mousePosition;
    Vector mouseWorldPosition;
    Scene scene;
    float elapsedTime;

  public:
    Game(int width, int height, const std::string& title, const std::string& shaderVertPath, const std::string& shaderFragPath);

    virtual void update() = 0;
    virtual void render() = 0;

    void run();
  };

}
