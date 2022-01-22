#pragma once

#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <string>
#include <unordered_map>

namespace flectron 
{

  class BaseShader
  {
  protected:
    GLuint rendererID;
    std::unordered_map<std::string, int> locationCache;

    int getUniformLocation(const std::string& name);

    std::string getSource(const std::string& filepath) const;
    unsigned int compileShader(unsigned int type, const std::string& source);
    unsigned int createShader(const std::string& vertexShader, const std::string& fragmentShader);

  public:
    ~BaseShader();

    void bind() const;
    void unbind() const;

    GLuint getRendererID() const;

    void setUniform1i(const std::string& name, int value);
    void setUniform1f(const std::string& name, float value);
    void setUniform2f(const std::string& name, float v1, float v2);
    void setUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
    void setUniformMat4f(const std::string& name, const glm::mat4& matrix);
    void setUniform1iv(const std::string& name, int* array, int size);
    void setUniform2fv(const std::string& name, float* array, int size);
    void setUniform3fv(const std::string& name, float* array, int size);
    void setUniform4fv(const std::string& name, float* array, int size);
  };

  class Shader : public BaseShader
  {
  private:
    std::string filepathVertex;
    std::string filepathFragment;

  public:
    Shader(const std::string& filepathVertex, const std::string& filepathFragment);
  };

  class ComputeShader : public BaseShader
  {
  private:
    std::string filepath;
    int maxWorkGroupCount[3];
    int maxWorkGroupSize[3];
    int maxWorkGroupInvocations;

  public:
    ComputeShader(const std::string& filepath);

    void dispatch(int x, int y, int z) const;
    void barrier() const;
  };

}