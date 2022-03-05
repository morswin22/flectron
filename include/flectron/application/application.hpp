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
      layers.push_back(new T(*this, std::forward<Args>(args)...));
      layers.back()->setup();
    }

    void popLayer();
  };

  Scope<Application> createApplication(const ApplicationArguments& arguments);

}
