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

  public:
    Game();

    virtual void update(float elapsedTime) = 0;
    virtual void render() = 0;

    void run();

    void wrapScreen();
    void removeOffscreen();
  };

};