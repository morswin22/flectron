#pragma once
#include <flectron/application/window.hpp>
#include <flectron/scene/scene.hpp>
#include <flectron/utils/stopwatch.hpp>

namespace flectron
{

  struct ApplicationArguments
  {
    int count;
    char** args;

    ApplicationArguments(int count, char** args);
    ApplicationArguments();

    const char* operator[](int index) const;
  };

  class Layer;

  class Application
  {
  public:
    ApplicationArguments arguments;
    Window window;
    float elapsedTime;
    Vector mousePosition;
    Vector mouseWorldPosition;

  private:
    std::vector<Layer*> layers;

  public:
    Application(const ApplicationArguments& arguments, const WindowProperties& properties);
    virtual ~Application();

    void run();

    template<typename T, typename ... Args>
    void pushLayer(Args&& ... args)
    {
      FLECTRON_LOG_TRACE("Pushing layer");
      layers.push_back(new T(*this, std::forward<Args>(args)...));
      layers.back()->setup();
      FLECTRON_LOG_INFO("Layer pushed");
    }

    void popLayer();

    size_t numLayers() const;
  };

  Scope<Application> createApplication(const ApplicationArguments& arguments);

}
