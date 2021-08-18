#pragma once

#include <MindlessEngine/window.hpp>
#include <MindlessEngine/world.hpp>

namespace MindlessEngine
{
  
  class Game
  {
  protected:
    Window window;
    Vector mousePosition;
    Vector mouseWorldPosition;
    World world;
    float elapsedTime;

  public:
    Game(int width, int height, const std::string& title, const std::string& shaderVertPath, const std::string& shaderFragPath);

    virtual void update() = 0;
    virtual void render() = 0;

    void run();

    void wrapScreen();
    void removeOffscreen();
  };

};