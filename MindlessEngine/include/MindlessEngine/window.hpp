#pragma once

#include <MindlessEngine/body.hpp>
#include <MindlessEngine/color.hpp>
#include <MindlessEngine/renderer.hpp>
#include <MindlessEngine/scene.hpp>

#include <string>
#include <memory>
#include <vector>

namespace MindlessEngine
{

  struct Constraints
  {
    float left;
    float right;
    float top;
    float bottom;

    Constraints(float left, float right, float top, float bottom);
  };

  Constraints operator+(const Constraints& constraints, const glm::vec3& offset);
  
  class Camera
  {
  private:
    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
    glm::mat4 viewProjectionMatrix;

    glm::vec3 position;
    float rotation;
  
    Constraints constraints;
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

    Constraints getConstraints() const;
    float getScale() const;

    void handleWASD();
    void handleScroll();
  };

  class Window
  {
  private:
    GLFWwindow* window;

  public:
    int width;
    int height;

  private:
    std::string title;

  public:
    Ref<Shader> shader;
    GLuint rendererBuffer;

  private:
    float desiredFrameRate;
    float desiredInterval;
    float lastMeasuredTime;

  public:
    Camera camera;

  public:
    Window(int width, int height, const std::string& title, const std::string& shaderVertPath, const std::string& shaderFragPath);
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

    void draw(Ref<Body>& body);
    void draw(const AABB& aabb, const Color& color);
    void draw(const Vector& a, const Vector& b, float weight, const Color& color);
    void draw(Ref<FontAtlas>& atlas, const Vector& position, const std::string& text, float scale, const Color& color);
    void draw(const Ref<LightScene>& scene, const Color& nightColor, float darkness);
  };

};
