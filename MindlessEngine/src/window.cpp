#include <MindlessEngine/window.hpp>

#include <MindlessEngine/input.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace MindlessEngine
{

  void GLAPIENTRY
  GLMessageCallback( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam )
  {
    if ( type == GL_DEBUG_TYPE_ERROR ) fprintf( stderr, "GL CALLBACK: type = 0x%x, severity = 0x%x, message = %s\n", type, severity, message );
  }
  
  Window::Window(int width, int height, const std::string& title) 
  : window(nullptr), width(width), height(height), title(title), lastMeasuredTime(0), 
    cameraPosition(glm::vec3(width * 0.5f, height * 0.5f, 0.0f)), 
    cameraScale(glm::vec3(1.0f, 1.0f, 1.0f)),
    projection(glm::ortho(0.0f, (float)width, 0.0f, (float)height, -1.0f, 1.0f)), 
    view(glm::scale(glm::translate(glm::mat4(1.0f), cameraPosition), cameraScale))
  {
    if (!glfwInit())
      return;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window)
    {
      glfwTerminate();
      return;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK)
      return;

    glEnable(GL_PROGRAM_POINT_SIZE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(GLMessageCallback, 0);

    glfwSetKeyCallback(window, keyboardCallback);
    glfwSetMouseButtonCallback(window, mouseCallback);
  }

  Window::~Window()
  {
    glfwDestroyWindow(window);
    glfwTerminate();
  }

  void Window::setTitle(const std::string& title)
  {
    this->title = title;
    glfwSetWindowTitle(window, title.c_str());
  }

  std::string Window::getTitle() const
  {
    return title;
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
    glfwGetFramebufferSize(window, &width, &height);
    // windowRatio = width / (float) height;
    glViewport(0, 0, width, height);
    projection = glm::ortho(0.0f, (float)width, 0.0f, (float)height, -1.0f, 1.0f);
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
    glfwPollEvents();
  }

  float Window::getElapsedTime()
  {
    float temp = lastMeasuredTime;
    lastMeasuredTime = (float) glfwGetTime();
    return lastMeasuredTime - temp;
  }

  glm::mat4 Window::getProjectionMatrix() const
  {
    return projection * view;
  }

  void Window::getCameraConstrains(float* left, float* top, float* right, float* bottom) const
  {
    *left = -cameraPosition.x / cameraScale.x;
    *top = cameraPosition.y / cameraScale.y;
    *right = cameraPosition.x / cameraScale.x;
    *bottom = -cameraPosition.y / cameraScale.y;
  }

  void Window::setScale(float x, float y)
  { 
    cameraScale.x = x;
    cameraScale.y = y;
    view = glm::scale(glm::translate(glm::mat4(1.0f), cameraPosition), cameraScale);
  }

  void Window::setScale(float a)
  {
    cameraScale.x = a;
    cameraScale.y = a;
    view = glm::scale(glm::translate(glm::mat4(1.0f), cameraPosition), cameraScale);
  }

  void Window::setSize(int width, int height)
  {
    glfwSetWindowSize(window, width, height);
    this->width = width;
    this->height = height;
    projection = glm::ortho(0.0f, (float)width, 0.0f, (float)height, -1.0f, 1.0f);
  }

};
