#include <flectron/application/application.hpp>
#include <flectron/utils/profile.hpp>
#include <flectron/utils/input.hpp>
#include <flectron/application/layer.hpp>
#include <flectron/assert/assert.hpp>

namespace flectron
{
  
  ApplicationArguments::ApplicationArguments(int count, char** args)
    : count(count), args(args)
  {
  }

  ApplicationArguments::ApplicationArguments()
    : count(0), args(nullptr)
  {
  }

  const char* ApplicationArguments::operator[](int index) const
  {
    if (index >= count)
      return nullptr;
    return args[index];
  }

  Application::Application(const ApplicationArguments& arguments, const WindowProperties& properties)
    : arguments(arguments), window(properties)
  {
  }

  Application::~Application()
  {
    for (auto layer : layers)
      delete layer;
  }

  void Application::run()
  {
    while (!window.shouldClose())
    {
      FLECTRON_PROFILE_FRAME("Application::run");
      elapsedTime = window.getElapsedTime();
      window.getCursorPosition(mousePosition);
      window.getFrameSize();

      const float scale = window.camera.getScale();
      const glm::vec3& cameraPosition = window.camera.getPosition();
      mouseWorldPosition.x = (mousePosition.x - window.properties.width * 0.5f) * scale + cameraPosition.x;
      mouseWorldPosition.y = (window.properties.height * 0.5f - mousePosition.y) * scale + cameraPosition.y;

      Renderer::setViewProjectionMatrix(window.camera);
      Renderer::beginBatch();
      for (auto layer : layers)
        layer->update();
      Renderer::endBatch();

      window.swapBuffers();
      window.pollEvents();
      window.regulateFrameRate();
    }
  }

  void Application::popLayer()
  {
    FLECTRON_ASSERT(layers.size() > 0, "No layer to pop");
    layers.back()->cleanup();
    layers.pop_back();
  }

}
