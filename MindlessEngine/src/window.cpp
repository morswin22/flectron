#include <MindlessEngine/window.hpp>

namespace MindlessEngine
{
  
  Window::Window(int width, int height, const std::string& title) : width(width), height(height), title(title)
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

  void Window::mainloop()
  {
    double mouseX, mouseY;

    while (!glfwWindowShouldClose(window))
    {
      glfwGetCursorPos(window, &mouseX, &mouseY);
      // mousePosition.x = (float) mouseX;
      // mousePosition.y = (float) mouseY;

      glfwGetFramebufferSize(window, &width, &height);
      // windowRatio = width / (float) height;

      glViewport(0, 0, width, height);
      glClear(GL_COLOR_BUFFER_BIT);

      // draw();

      glfwSwapBuffers(window);
      glfwPollEvents();
    }
  }

};
