#include <flectron/scene/game.hpp>
#include <flectron/utils/input.hpp>
#include <flectron/utils/profile.hpp>

namespace flectron
{
  
  Game::Game(int width, int height, const std::string& title, const std::string& shaderVertPath, const std::string& shaderFragPath) 
    : window(width, height, title, shaderVertPath, shaderFragPath),
      mousePosition(), mouseWorldPosition(), 
      scene(),
      elapsedTime(0.0f)
  {}

  void Game::run()
  {
    while (!window.shouldClose())
    {
      FLECTRON_PROFILE_FRAME("Game::run");
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

}
