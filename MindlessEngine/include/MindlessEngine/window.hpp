#pragma once

#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <string>

#include <vector>
#include <MindlessEngine/body.hpp>

namespace MindlessEngine
{
  
  class Window
  {
  private:
    GLFWwindow* window;

  public:
    int width;
    int height;

  private:
    std::string title;
    float lastMeasuredTime;

    glm::vec3 cameraPosition;
    glm::vec3 cameraScale;

    glm::mat4 projection;
    glm::mat4 view;

  public:
    Window(int width, int height, const std::string& title);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    void setTitle(const std::string& title);
    std::string getTitle() const;

    bool shouldClose() const;
    void getCursorPosition(Vector& v) const;
    void getFrameSize();
    void clear() const;
    void swapBuffers() const;
    void pollEvents() const;

    float getElapsedTime();
    glm::mat4 getProjectionMatrix() const;

    void getCameraConstrains(float* left, float* top, float* right, float* bottom) const;

    void setScale(float x, float y);
    void setScale(float a);

    void setSize(int width, int height);
  };

};
