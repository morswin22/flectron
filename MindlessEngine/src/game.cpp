#include <MindlessEngine/game.hpp>

namespace MindlessEngine
{
  
  Game::Game() : window(640, 480, "Mindless Game"), mousePosition(), mouseWorldPosition(), world() {}

  void Game::run()
  {
    window.getElapsedTime();
    while (!window.shouldClose())
    {
      window.getCursorPosition(mousePosition);
      window.getFrameSize();

      mouseWorldPosition.x = (mousePosition.x - window.cameraPosition.x) / window.cameraScale.x;
      mouseWorldPosition.y = -(mousePosition.y - window.cameraPosition.y) / window.cameraScale.y;

      update(window.getElapsedTime());

      Renderer::beginBatch();
      render();
      Renderer::endBatch();
      Renderer::flush();

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

  void Game::removeOffscreen()
  {
    float left, top, right, bottom;
    window.getCameraConstrains(&left, &top, &right, &bottom);

    for (int i = world.getBodyCount() - 1; i >= 0; i--)
    {
      Body& body = world.getBody(i);

      if (body.position.x < left || body.position.x > right ||
          body.position.y < bottom || body.position.y > top)
      {
        world.removeBody(i);
      }
    }
  }

};