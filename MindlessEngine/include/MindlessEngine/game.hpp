#pragma once

#include <MindlessEngine/window.hpp>

namespace MindlessEngine
{
  
  class Game
  {
  protected:
    Window window;
    Vector mousePosition;

  public:
    Game();

    virtual void update(float elapsedTime) = 0;
    virtual void render() = 0;

    void run();
  };

};