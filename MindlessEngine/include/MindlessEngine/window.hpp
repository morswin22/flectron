#pragma once

#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <string>

namespace MindlessEngine
{
  
  class Window
  {
  private:
    GLFWwindow* window;

  public:
    int width;
    int height;
    std::string title;

    Window(int width, int height, const std::string& title);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    void setTitle(const std::string& title);
    std::string getTitle() const;

    void mainloop();
  };

};
