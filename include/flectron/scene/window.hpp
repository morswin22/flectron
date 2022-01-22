#pragma once

#include <flectron/renderer/color.hpp>
#include <flectron/renderer/renderer.hpp>
#include <flectron/renderer/light.hpp>
#include <flectron/generation/wfc.hpp>
#include <flectron/scene/camera.hpp>

#include <string>
#include <memory>
#include <vector>

namespace flectron
{

  class Entity;

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
    void regulateFrameRate();

    void draw(Entity body);
    void draw(Ref<Body>& body);
    void draw(const AABB& aabb, const Color& color);
    void draw(const Vector& a, const Vector& b, float weight, const Color& color);
    void draw(Ref<FontAtlas>& atlas, const Vector& position, const std::string& text, float scale, const Color& color);
    void draw(const Scope<LightRenderer>& scene, const Color& nightColor, float darkness);
    void draw(const Vector& position, float size, const WFC::Tile* tile, const Color& color);
  };

}
