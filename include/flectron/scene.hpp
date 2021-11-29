#pragma once

#include <flectron/renderer.hpp>
#include <flectron/memory.hpp>
#include <flectron/vector.hpp>
#include <flectron/color.hpp>
#include <flectron/body.hpp>

namespace flectron
{

  class LightScene
  {
  private:
    Ref<Shader> shader;
    float lightsData[128 * 3];
    float lightsColors[128 * 4];

    int currentLight;
    int currentData;
    int currentColor;

    GLuint vao;
    GLuint vbo;
    GLuint ibo;

  public:
    LightScene(const std::string& vertexPath, const std::string& fragmentPath);
    ~LightScene();

    void reset();

    void render(const Color& baseColor, float darkness, const glm::vec3& cameraPosition, const glm::vec2& windowSize, GLuint rendererBuffer);

    void addLight(const Vector& position, float radius, const Color& color);
    void addLight(const Ref<Body>& body);
  };

  class TimeScene
  {
  private:
    float time;
    float scale;

    float timeZones[4];
    float transitionTime;

    float minDarkness;
    float maxDarkness;

    int day;

  public:
    TimeScene(float startTime, float nightToDay, float dayToNight, float transitionTime, float scale, float minDarkness, float maxDarkness);

    void reset();
    void setTime(float time);
    void setScale(float scale);

    int getDay() const;
    float getTime() const;
    float getScale() const;

    float getDarkness() const;

    void update(float delta);
  };

}
