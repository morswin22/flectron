#pragma once

#include <MindlessEngine/body.hpp>
#include <MindlessEngine/color.hpp>
#include <MindlessEngine/renderer.hpp>

#include <string>
#include <memory>
#include <vector>

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

  private:
    float desiredFrameRate;
    float desiredInterval;
    float lastMeasuredTime;

  public:
    glm::vec3 cameraPosition;
    glm::vec3 cameraScale;

  private:
    glm::mat4 projection;
    glm::mat4 view;

  public:
    Window(int width, int height, const std::string& title);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;

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
    void setBackground(const Color& color);

    void setDesiredFrameRate(float desiredFrameRate);
    float getDesiredFrameRate() const;

    void draw(Body& body);
    void draw(const Vector& a, const Vector& b, float weight, const Color& color);
  };

};
