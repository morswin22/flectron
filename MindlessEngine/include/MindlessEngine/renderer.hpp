#pragma once

#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>
#include <string>
#include <unordered_map>

namespace MindlessEngine
{

  class Shader
  {
  private:
    GLuint rendererID;
    std::string filepathVertex;
    std::string filepathFragment;
    std::unordered_map<std::string, int> locationCache;

    int getUniformLocation(const std::string& name);

    std::string getSource(const std::string& filepath) const;
    unsigned int compileShader(unsigned int type, const std::string& source);
    unsigned int createShader(const std::string& vertexShader, const std::string& fragmentShader);

  public:
    Shader(const std::string& filepathVertex, const std::string& filepathFragment);
    ~Shader();

    void bind() const;
    void unbind() const;

    void setUniform1i(const std::string& name, int value);
    void setUniform1f(const std::string& name, float value);
    void setUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
    void setUniformMat4f(const std::string& name, const glm::mat4& matrix);
  };

  class Vector;
  class Color;
  class Body;

  class Renderer
  {
  public:
    static void init();
    static void shutdown();

    static void beginBatch();
    static void endBatch();
    static void flush();

    static void draw(const Vector* vertices, int numVertices, const uint32_t* triangles, const Color& color);
  };

};