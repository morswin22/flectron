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
      GLuint rendererID;

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
    std::unordered_map<std::string, GLuint> uniformBlockBindings;
    Shaders shaders;

    static GLint maxUniformBlockBinings;
    static GLint getMaxUniformBlockBinings();

    int getUniformLocation(const std::string& name);
    int getUniformBlockIndex(const std::string& name);
    GLuint getUniformBlockBinding(const std::string& name);
    static GLuint compileShader(GLenum type, const std::string& source);

    void linkAndValidate();
    static void checkShaderError(GLuint rendererID, GLenum error, const std::string& format);
    static void checkProgramError(GLuint rendererID, GLenum error, const std::string& format);

  public:
    Shader();
    Shader(const Shaders& shaders);
    ~Shader();

    static void init();
    static Ref create(const Shaders& shaders);

    void addShader(ShaderType type, const TextView& source);
    
    void reload();
    void resetUniformBlockBindings();

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
    void setUniformBlock(const std::string& name, GLuint bindingPoint);

    // Compute shader specific
    struct WorkGroupInfo
    {
      int maxCount[3];
      int maxSize[3];
      int maxInvocations;
    };

    static WorkGroupInfo workGroupInfo;
    static WorkGroupInfo getWorkGroupInfo();
    void dispatch(int x, int y, int z) const;
    void barrier() const;
  };

}