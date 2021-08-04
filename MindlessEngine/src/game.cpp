#include <MindlessEngine/game.hpp>

namespace MindlessEngine
{
  
  Game::Game() : window(640, 480, "Mindless Game"), mousePosition() {}

  void Game::run()
  {
    window.getElapsedTime();
    while (!window.shouldClose())
    {
      window.getCursorPosition(mousePosition);
      window.getFrameSize();

      update(window.getElapsedTime());

      window.clear();

      render();

      window.swapBuffers();
      window.pollEvents();
    }
  }

};