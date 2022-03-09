#pragma once

#include <flectron/renderer/color.hpp>
#include <flectron/renderer/renderer.hpp>
#include <flectron/application/camera.hpp>

#include <string>
#include <memory>
#include <vector>

namespace flectron
{

  enum WindowFlags
  {
    FULLSCREEN = 0x1, VSYNC = 0x2
  };

  struct WindowProperties
  {
    std::string title;
    int width;
    int height;
    float fps;
    bool fullscreen;
    bool vsync;

    WindowProperties(const std::string& title, WindowFlags flags);
    WindowProperties(const std::string& title, int width, int height, WindowFlags flags);
    WindowProperties(const std::string& title, int width, int height, float fps);
  };

  class Entity;

  class Window
  {
  public:
    WindowProperties properties;

  private:
    GLFWwindow* window;

  public:
    GLuint rendererBuffer;

  private:
    float desiredInterval;
    float lastMeasuredTime;

  public:
    Camera camera;

  public:
    Window(const WindowProperties& properties);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;

    void setTitle(const std::string& title);
    std::string getTitle() const;

    GLuint getRendererBuffer() const;

    bool shouldClose() const;
    void getCursorPosition(Vector& v) const;
    void getFrameSize();
    void clear() const;
    void swapBuffers() const;
    void pollEvents() const;

    float getElapsedTime();

    void setSize(int width, int height);
    void setBackground(const Color& color);

    void setDesiredFrameRate(float desiredFrameRate);
    float getDesiredFrameRate() const;
    void regulateFrameRate();
  };

}
