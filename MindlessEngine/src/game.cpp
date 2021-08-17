#include <MindlessEngine/game.hpp>
#include <MindlessEngine/input.hpp>

#include <iostream>

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

      const float scale = window.camera.getScale();
      const glm::vec3& cameraPosition = window.camera.getPosition();
      mouseWorldPosition.x = (mousePosition.x - window.width * 0.5f) * scale + cameraPosition.x;
      mouseWorldPosition.y = (window.height * 0.5f - mousePosition.y) * scale + cameraPosition.y;

      elapsedTime = window.getElapsedTime();
      update();

      Renderer::beginBatch();
      render();
      Renderer::endBatch();
      Renderer::flush();
      
      Mouse::resetScroll();

      window.swapBuffers();
      window.pollEvents();
    }
  }

  void Game::wrapScreen()
  {
    const glm::vec4& contraints = window.camera.getConstraints();

    for (int i = 0; i < world.getBodyCount(); i++)
    {
      Body& body = world.getBody(i);

      if (body.isStatic)
        continue;

      const AABB& box = body.getAABB();

      if (box.max.x < contraints.s)
        body.moveTo({ contraints.t, body.position.y });

      if (box.min.x > contraints.t)
        body.moveTo({ contraints.s, body.position.y });

      if (box.max.y < contraints.p)
        body.moveTo({ body.position.x, contraints.q });

      if (box.min.y > contraints.q)
        body.moveTo({ body.position.x, contraints.p });
    }
  }

  void Game::removeOffscreen()
  {
    const glm::vec4& contraints = window.camera.getConstraints();

    for (int i = world.getBodyCount() - 1; i >= 0; i--)
    {
      Body& body = world.getBody(i);

      if (body.isStatic)
        continue;

      const AABB& box = body.getAABB();

      if (box.max.x < contraints.s || box.min.x > contraints.t ||
          box.max.y < contraints.p || box.min.y > contraints.q)
      {
        world.removeBody(i);
      }
    }
  }

};