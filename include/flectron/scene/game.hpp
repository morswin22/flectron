#pragma once

#include <flectron/scene/window.hpp>
#include <flectron/scene/scene.hpp>
#include <flectron/utils/stopwatch.hpp>

#define FLECTRON_PHYSICS 100
#define FLECTRON_RENDER 200

namespace flectron
{
  
  class Game
  {
  private:
    Stopwatch physicsTimer;

  protected:
    Window window;
    Vector mousePosition;
    Vector mouseWorldPosition;
    Scene scene;
    float elapsedTime;
    float physicsTime;
    size_t physicsIterations;

  public:
    Game(int width, int height, const std::string& title, const std::string& shaderVertPath, const std::string& shaderFragPath);

    void run();
  };

}
