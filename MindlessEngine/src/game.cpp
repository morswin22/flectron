#include <MindlessEngine/game.hpp>
#include <MindlessEngine/input.hpp>
#include <MindlessEngine/profile.hpp>

namespace MindlessEngine
{
  
  Game::Game(int width, int height, const std::string& title, const std::string& shaderVertPath, const std::string& shaderFragPath) 
    : window(width, height, title, shaderVertPath, shaderFragPath),
      mousePosition(), mouseWorldPosition(), 
      world(),
      elapsedTime(0.0f)
  {}

  void Game::run()
  {
    while (!window.shouldClose())
    {
      ME_PROFILE_FRAME("Game::run");
      elapsedTime = window.getElapsedTime();
      window.getCursorPosition(mousePosition);
      window.getFrameSize();

      const float scale = window.camera.getScale();
      const glm::vec3& cameraPosition = window.camera.getPosition();
      mouseWorldPosition.x = (mousePosition.x - window.width * 0.5f) * scale + cameraPosition.x;
      mouseWorldPosition.y = (window.height * 0.5f - mousePosition.y) * scale + cameraPosition.y;

      update();

      window.shader->bind();
      window.shader->setUniformMat4f("uViewProjection", window.camera.getViewProjectionMatrix());
      Renderer::beginBatch();
      render();
      Renderer::endBatch();
      
      Mouse::resetScroll();

      window.swapBuffers();
      window.pollEvents();
      window.regulateFrameRate();
    }
  }

  void Game::wrapScreen()
  {
    const Constraints& cc = window.camera.getConstraints();

    for (int i = 0; i < world.getBodyCount(); i++)
    {
      Ref<Body>& body = world.getBody(i);

      if (body->isStatic)
        continue;

      const AABB& box = body->getAABB();

      if (box.max.x < cc.left)
        body->moveTo({ cc.right, body->position.y });

      if (box.min.x > cc.right)
        body->moveTo({ cc.left, body->position.y });

      if (box.max.y < cc.top)
        body->moveTo({ body->position.x, cc.bottom });

      if (box.min.y > cc.bottom)
        body->moveTo({ body->position.x, cc.top });
    }
  }

  void Game::removeOffscreen()
  {
    const Constraints& cc = window.camera.getConstraints();

    for (int i = world.getBodyCount() - 1; i >= 0; i--)
    {
      Ref<Body>& body = world.getBody(i);

      if (body->isStatic)
        continue;

      const AABB& box = body->getAABB();

      if (box.max.x < cc.left || box.min.x > cc.right ||
          box.max.y < cc.top  || box.min.y > cc.bottom)
      {
        world.removeBody(i);
      }
    }
  }

};