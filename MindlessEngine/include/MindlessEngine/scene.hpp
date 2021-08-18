#pragma once

#include <MindlessEngine/renderer.hpp>
#include <MindlessEngine/memory.hpp>
#include <MindlessEngine/vector.hpp>
#include <MindlessEngine/color.hpp>
#include <MindlessEngine/body.hpp>

namespace MindlessEngine
{

  class LightScene
  {
  private:
    GLuint buffer;
    Ref<ComputeShader> shader;
    float lightsData[128 * 3];
    // float lightsColors[128 * 4];

    int currentLight;
    int currentData;
    // int currentColor;

  public:
    int width, height;

  public:
    LightScene(std::string path, int width, int height);
    ~LightScene();

    GLuint getBuffer() const;

    void reset();

    void compute(const Color& baseColor, const glm::vec3& cameraPosition, float cameraScale);

    void addLight(const Vector& position, float radius);
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

};