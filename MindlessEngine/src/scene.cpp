#include <MindlessEngine/scene.hpp>

#include <MindlessEngine/math.hpp>

namespace MindlessEngine
{

  LightScene::LightScene(std::string path, std::string vertexPath, std::string fragmentPath, int width, int height)
    : shader(createRef<ComputeShader>(path)), 
      combineShader(createRef<Shader>(vertexPath, fragmentPath)),
      currentLight(0), currentData(0), currentColor(0),
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

    constexpr float positions[]{
      -1.0f, -1.0f,
      -1.0f,  1.0f,
       1.0f,  1.0f,
       1.0f, -1.0f
    };

    constexpr unsigned int indices[]{
      0, 1, 2,
      2, 3, 0
    };

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (const void*)0);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    combineShader->bind();
    combineShader->setUniform1i("uRendererTexture", 0);
    combineShader->setUniform1i("uLightTexture", 1);
  }

  LightScene::~LightScene()
  {
    glDeleteTextures(1, &buffer);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);
    glDeleteVertexArrays(1, &vao);
  }

  GLuint LightScene::getBuffer() const
  {
    return buffer;
  }

  void LightScene::reset()
  {
    currentLight = 0;
    currentData = 0;
    currentColor = 0;
  }

  void LightScene::render(const Color& baseColor, float darkness, const glm::vec3& cameraPosition, float cameraScale, GLuint rendererBuffer)
  {
    Renderer::endBatch();

    // TODO probably there is no need for a compute shader.
    // The fragment shader can calculate the light values on the fly
    // and then combine them with the renderer's offscreen buffer

    shader->bind();
    shader->setUniform2f("uCameraPosition", cameraPosition.x, cameraPosition.y);
    shader->setUniform1f("uCameraScale", cameraScale);
    shader->setUniform4f("uBaseColor", baseColor.r, baseColor.g, baseColor.b, baseColor.a);

    shader->setUniform4fv("uLightColor", lightsColors, currentLight);
    shader->setUniform3fv("uLightData", lightsData, currentLight);
    shader->setUniform1i("uLightCount", currentLight);

    shader->dispatch(width, height, 1);

    reset();

    shader->barrier();
    combineShader->bind();
    combineShader->setUniform1f("uDarkness", darkness);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTextureUnit(0, rendererBuffer);
    glBindTextureUnit(1, buffer);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    Renderer::beginBatch();
  }

  void LightScene::addLight(const Vector& position, float radius, const Color& color)
  {
    if (currentLight >= 128)
      throw std::runtime_error("Too many lights");

    lightsData[currentData++] = position.x;
    lightsData[currentData++] = position.y;
    lightsData[currentData++] = radius;

    lightsColors[currentColor++] = color.r;
    lightsColors[currentColor++] = color.g;
    lightsColors[currentColor++] = color.b;
    lightsColors[currentColor++] = color.a;

    currentLight += 1;
  }

  void LightScene::addLight(const Ref<Body>& body)
  {
    if (currentLight + 3 >= 128 * 3)
      throw std::runtime_error("Too many lights");

    lightsData[currentData++] = body->position.x;
    lightsData[currentData++] = body->position.y;
    lightsData[currentData++] = body->lightRadius;

    lightsColors[currentColor++] = body->lightColor.r;
    lightsColors[currentColor++] = body->lightColor.g;
    lightsColors[currentColor++] = body->lightColor.b;
    lightsColors[currentColor++] = body->lightColor.a;
    
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