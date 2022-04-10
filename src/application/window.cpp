#include <flectron/application/window.hpp>

#include <flectron/utils/input.hpp>
#include <flectron/physics/math.hpp>
#include <flectron/renderer/renderer.hpp>
#include <flectron/utils/profile.hpp>
#include <flectron/scene/entity.hpp>
#include <flectron/assert/assert.hpp>
#include <sstream>

namespace flectron
{

  void GLAPIENTRY
  GLMessageCallback( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei, const GLchar* message, const void* )
  {
    if (type == GL_DEBUG_TYPE_ERROR) fprintf(stderr, "GL CALLBACK: source = 0x%x, type = 0x%x, id = 0x%x, severity = 0x%x, message = %s\n", source, type, id, severity, message);
  }
  
  WindowProperties::WindowProperties(const std::string& title, WindowFlags flags)
    : title(title), width(800), height(600), fps(60.0f), fullscreen(flags & FULLSCREEN), vsync(flags & VSYNC)
  {
  }

  WindowProperties::WindowProperties(const std::string& title, int width, int height, WindowFlags flags)
    : title(title), width(width), height(height), fps(60.0f), fullscreen(false), vsync(flags & VSYNC)
  {
  }

  WindowProperties::WindowProperties(const std::string& title, int width, int height, float fps)
    : title(title), width(width), height(height), fps(fps), fullscreen(false), vsync(false)
  {
  }

  Window::Window(const WindowProperties& properties) 
  : properties(properties), window(nullptr),
    desiredInterval(1.0f / properties.fps), lastMeasuredTime(0), 
    camera(-properties.width * 0.5f, properties.width * 0.5f, -properties.height * 0.5f, properties.height * 0.5f)
  {
    if (!glfwInit())
      return;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    window = glfwCreateWindow(properties.width, properties.height, properties.title.c_str(), nullptr, nullptr);
    if (!window)
    {
      glfwTerminate();
      return;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

    if (glewInit() != GLEW_OK)
      return;

    glEnable(GL_PROGRAM_POINT_SIZE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(GLMessageCallback, 0);

    Keyboard::init(window);
    Mouse::init(window);

    Renderer::init(properties.width, properties.height, rendererBuffer);
  }

  Window::~Window()
  {
    Renderer::shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();
  }

  void Window::setTitle(const std::string& title)
  {
    properties.title = title;
    glfwSetWindowTitle(window, title.c_str());
  }

  std::string Window::getTitle() const
  {
    return properties.title;
  }

  GLuint Window::getRendererBuffer() const
  {
    return rendererBuffer;
  }

  bool Window::shouldClose() const
  {
    return glfwWindowShouldClose(window);
  }
  void Window::getCursorPosition(Vector& v) const
  {
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    v.x = (float) x;
    v.y = (float) y;
  }

  void Window::getFrameSize()
  {
    int newWidth, newHeight;
    glfwGetFramebufferSize(window, &newWidth, &newHeight);
    if (newWidth == properties.width && newHeight == properties.height)
      return;

    properties.width = newWidth;
    properties.height = newHeight;
    glViewport(0, 0, properties.width, properties.height);
    const float scale = camera.getScale();
    camera.setProjection(-properties.width * 0.5f * scale, properties.width * 0.5f * scale, -properties.height * 0.5f * scale, properties.height * 0.5f * scale);
    glBindTexture(GL_TEXTURE_2D, rendererBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, properties.width, properties.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  void Window::clear() const
  {
    glClear(GL_COLOR_BUFFER_BIT);
  }

  void Window::swapBuffers() const
  {
    glfwSwapBuffers(window);
  }

  void Window::pollEvents() const
  {
    Keyboard::reset();
    Mouse::reset();
    glfwPollEvents();
  }

  float Window::getElapsedTime()
  {
    float temp = lastMeasuredTime;
    lastMeasuredTime = (float) glfwGetTime();
    return lastMeasuredTime - temp;
  }

  void Window::setSize(int width, int height)
  {
    FLECTRON_ASSERT(width > 0 && height > 0, "Invalid window size");
    glfwSetWindowSize(window, width, height);
    glViewport(0, 0, width, height);
    properties.width = width;
    properties.height = height;
    const float scale = camera.getScale();
    camera.setProjection(-width * 0.5f * scale, width * 0.5f * scale, -height * 0.5f * scale, height * 0.5f * scale);
    glBindTexture(GL_TEXTURE_2D, rendererBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  void Window::setBackground(const Color& color)
  {
    glClearColor(color.r, color.g, color.b, color.a);
  }

  void Window::setDesiredFrameRate(float desiredFrameRate)
  {
    FLECTRON_ASSERT(desiredFrameRate > 0.0f, "Desired frame rate must be greater than zero");
    properties.fps = desiredFrameRate;
    desiredInterval = 1.0f / properties.fps;
  }

  float Window::getDesiredFrameRate() const
  {
    return properties.fps;
  }

  void Window::regulateFrameRate()
  {
    FLECTRON_PROFILE_EVENT("Window::regulateFrameRate");
    while ((float) glfwGetTime() - lastMeasuredTime < desiredInterval && !shouldClose())
      glfwPollEvents();
  }

}
