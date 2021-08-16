#pragma once

#include <MindlessEngine/body.hpp>
#include <MindlessEngine/color.hpp>
#include <MindlessEngine/renderer.hpp>

#include <string>
#include <memory>
#include <vector>

namespace MindlessEngine
{
  
  class Camera
  {
  private:
    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
    glm::mat4 viewProjectionMatrix;

    glm::vec3 position;
    float rotation;
  
    glm::vec4 constraints;
    float scale;

    void recalculateViewMatrix();

  public:
    Camera(float left, float right, float bottom, float top);

    const glm::vec3& getPosition() const;
    float getRotation() const;

    void setProjection(float left, float right, float bottom, float top);
    void setPosition(const glm::vec3& position);
    void setRotation(float rotation);
    void setScale(float scale);

    void moveTo(const Vector& position);
    void move(const Vector& amount);

    const glm::mat4& getProjectionMatrix() const;
    const glm::mat4& getViewMatrix() const;
    const glm::mat4& getViewProjectionMatrix() const;

    glm::vec4 getConstraints() const;
    float getScale() const;
  };

  class Window
  {
  private:
    GLFWwindow* window;

  public:
    int width;
    int height;
    int maxTextureSlots;

  private:
    std::string title;

  private:
    float desiredFrameRate;
    float desiredInterval;
    float lastMeasuredTime;

  public:
    Camera camera;

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

    void setSize(int width, int height);
    void setBackground(const Color& color);

    void setDesiredFrameRate(float desiredFrameRate);
    float getDesiredFrameRate() const;

    void draw(Body& body);
    void draw(const Vector& a, const Vector& b, float weight, const Color& color);
    void draw(FontAtlas& atlas, const Vector& position, const std::string& text, float scale, const Color& color);
  };

};
