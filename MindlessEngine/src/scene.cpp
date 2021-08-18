#include <MindlessEngine/scene.hpp>

#include <MindlessEngine/math.hpp>

namespace MindlessEngine
{

  LightScene::LightScene(std::string path, int width, int height)
    : shader(createRef<ComputeShader>(path)), 
      currentLight(0), currentData(0),// currentColor(0),
      width(width), height(height)
  {
    glGenTextures(1, &buffer);
    glBindTexture(GL_TEXTURE_2D, buffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glBindImageTexture(0, buffer, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
  }

  LightScene::~LightScene()
  {
    glDeleteTextures(1, &buffer);
  }

  GLuint LightScene::getBuffer() const
  {
    return buffer;
  }

  void LightScene::reset()
  {
    currentLight = 0;
    currentData = 0;
    // currentColor = 0;
  }

  void LightScene::compute(const Color& baseColor, const glm::vec3& cameraPosition, float cameraScale)
  {
    shader->bind();
    shader->setUniform2f("uCameraPosition", cameraPosition.x, cameraPosition.y);
    shader->setUniform1f("uCameraScale", cameraScale);
    shader->setUniform4f("uBaseColor", baseColor.r, baseColor.g, baseColor.b, baseColor.a);

    // shader->setUniform4fv("uLightColor", lightsColors, currentLight);
    shader->setUniform3fv("uLightData", lightsData, currentLight);
    shader->setUniform1i("uLightCount", currentLight);

    shader->dispatch(width, height, 1);

    reset();

    shader->barrier(); // TODO figure out when to call this
  }

  void LightScene::addLight(const Vector& position, float radius)
  {
    if (currentLight >= 128)
      throw std::runtime_error("Too many lights");

    lightsData[currentData++] = position.x;
    lightsData[currentData++] = position.y;
    lightsData[currentData++] = radius;

    // lightsColors[currentColor++] = color.r;
    // lightsColors[currentColor++] = color.g;
    // lightsColors[currentColor++] = color.b;
    // lightsColors[currentColor++] = color.a;

    currentLight += 1;
  }

  void LightScene::addLight(const Ref<Body>& body)
  {
    if (currentLight + 3 >= 128 * 3)
      throw std::runtime_error("Too many lights");

    lightsData[currentData++] = body->position.x;
    lightsData[currentData++] = body->position.y;
    lightsData[currentData++] = body->lightRadius;
    
    currentLight += 1;
  }

  TimeScene::TimeScene(float startTime, float nightToDay, float dayToNight, float transitionTime, float scale, float minDarkness, float maxDarkness)
    : time(startTime), scale(scale), transitionTime(transitionTime), minDarkness(minDarkness), maxDarkness(maxDarkness), day(0)
  {
    timeZones[0] = nightToDay;
    timeZones[1] = nightToDay + transitionTime;
    timeZones[2] = dayToNight;
    timeZones[3] = dayToNight + transitionTime;
  }

  void TimeScene::reset()
  {
    time = 0.0f;
  }

  void TimeScene::setTime(float time)
  {
    this->time = time;
  }

  void TimeScene::setScale(float scale)
  {
    this->scale = scale;
  }

  int TimeScene::getDay() const
  {
    return day;
  }

  float TimeScene::getTime() const
  {
    return time;
  }

  float TimeScene::getScale() const
  {
    return scale;
  }

  float TimeScene::getDarkness() const
  {
    if (time < timeZones[0])
      return maxDarkness;
    if (time < timeZones[1])
      return 1.0f - clamp((time - timeZones[0]) / transitionTime, 1.0f - maxDarkness, 1.0f - minDarkness);
    if (time < timeZones[2])
      return minDarkness;
    if (time < timeZones[3])
      return clamp((time - timeZones[2]) / transitionTime, minDarkness, maxDarkness);
    return maxDarkness;
  }

  void TimeScene::update(float delta)
  {
    time += delta * scale;
    if (time >= 1.0f)
    {
      int days = (int)time;
      day += days;
      time = time - (float)days;
    }
  }

};