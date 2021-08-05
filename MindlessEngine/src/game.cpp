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

  void Game::wrapScreen()
  {
    float left, top, right, bottom;
    window.getCameraConstrains(&left, &top, &right, &bottom);

    for (int i = 0; i < world.getBodyCount(); i++)
    {
      Body& body = world.getBody(i);

      if (body.position.x < left)
        body.moveTo({ right, body.position.y });

      if (body.position.x > right)
        body.moveTo({ left, body.position.y });

      if (body.position.y < bottom)
        body.moveTo({ body.position.x, top });

      if (body.position.y > top)
        body.moveTo({ body.position.x, bottom });
    }
  }

};