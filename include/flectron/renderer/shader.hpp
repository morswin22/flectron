#pragma once

#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <string>
#include <unordered_map>
#include <flectron/assets/text.hpp>

namespace flectron 
{

  class Shader
  {
  public:
    using Ref = Ref<Shader>;

    enum ShaderType
    {
      VERTEX = GL_VERTEX_SHADER,
      GEOMETRY = GL_GEOMETRY_SHADER,
      FRAGMENT = GL_FRAGMENT_SHADER,
      COMPUTE = GL_COMPUTE_SHADER
    };

    struct Shaders
    {
      TextView vertex;
      TextView geometry;
      TextView fragment;
      TextView compute;
    };

  private:
    struct ShadersAttacher
    {
      GLuint vertex;
      GLuint geometry;
      GLuint fragment;
      GLuint compute;

      ShadersAttacher(GLuint rendererID, const Shaders& shaders);
      ~ShadersAttacher();
    };

  private:
    GLuint rendererID;
    std::unordered_map<std::string, int> locationCache;
    Shaders shaders;

    int getUniformLocation(const std::string& name);
    static GLuint compileShader(GLenum type, const std::string& source);

  public:
    Shader();
    Shader(const Shaders& shaders);
    ~Shader();

    static Ref create(const Shaders& shaders);

    void addShader(ShaderType type, const TextView& source);
    
    void reload();

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

    // Compute shader specific
    struct WorkGoupInfo
    {
      int maxWorkGroupCount[3];
      int maxWorkGroupSize[3];
      int maxWorkGroupInvocations;
    };

    WorkGoupInfo getWorkGroupInfo() const;
    void dispatch(int x, int y, int z) const;
    void barrier() const;
  };

}