#include <flectron/renderer/shader.hpp>
#include <flectron/assert/assert.hpp>

#include <sstream>
#include <fstream>
#include <string>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

namespace flectron 
{

  Shader::~Shader()
  {
    glDeleteProgram(rendererID);
  }

  void Shader::bind() const
  {
    glUseProgram(rendererID);
  }

  void Shader::unbind() const
  {
    glUseProgram(0);
  }

  GLuint Shader::getRendererID() const
  {
    return rendererID;
  }

  void Shader::setUniform1i(const std::string& name, int value)
  {
    glUniform1i(getUniformLocation(name), value);
  }

  void Shader::setUniform1f(const std::string& name, float value)
  {
    glUniform1f(getUniformLocation(name), value);
  }
  
  void Shader::setUniform2f(const std::string& name, float v1, float v2)
  {
    glUniform2f(getUniformLocation(name), v1, v2);
  }

  void Shader::setUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
  {
    glUniform4f(getUniformLocation(name), v0, v1, v2, v3);
  }

  void Shader::setUniformMat4f(const std::string& name, const glm::mat4& matrix)
  {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
  }

  void Shader::setUniform1iv(const std::string& name, int* array, int size)
  {
    glUniform1iv(getUniformLocation(name), size, array);
  }

  void Shader::setUniform2fv(const std::string& name, float* array, int size)
  {
    glUniform2fv(getUniformLocation(name), size, array);
  }

  void Shader::setUniform3fv(const std::string& name, float* array, int size)
  {
    glUniform3fv(getUniformLocation(name), size, array);
  }

  void Shader::setUniform4fv(const std::string& name, float* array, int size)
  {
    glUniform4fv(getUniformLocation(name), size, array);
  }

  int Shader::getUniformLocation(const std::string& name)
  {
    if (locationCache.find(name) != locationCache.end())
      return locationCache[name];

    int location = glGetUniformLocation(this->rendererID, name.c_str());
    FLECTRON_ASSERT(location != -1, "Uniform " + name + " not found");
    
    locationCache[name] = location;
    return location;
  }

  unsigned int Shader::compileShader(unsigned int type, const std::string& source)
  {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
      int length;
      glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
      char* message = (char*)alloca(length * sizeof(char));
      glGetShaderInfoLog(id, length, &length, message);
      std::stringstream ss;
      ss << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
      ss << message << std::endl;
      glDeleteShader(id);
      FLECTRON_ASSERT(false, ss.str());
    }

    return id;
  }

  Shader::Shader()
    : rendererID(0u), locationCache(), shaders({ nullptr, nullptr, nullptr, nullptr })
  {
    FLECTRON_LOG_TRACE("Creating empty shader");
    rendererID = glCreateProgram();
    glLinkProgram(rendererID);
    glValidateProgram(rendererID);
  }

  Shader::Shader(const Shaders& shaders)
    : rendererID(0u), locationCache(), shaders(shaders)
  {
    FLECTRON_LOG_TRACE("Creating shader");
    rendererID = glCreateProgram();

    ShadersAttacher attacher(rendererID, shaders);

    glLinkProgram(rendererID);
    glValidateProgram(rendererID);
  }

  Ref<Shader> Shader::create(const Shaders& shaders)
  {
    return createRef<Shader>(shaders);
  }

  void Shader::addShader(ShaderType type, const TextView& source)
  {
    switch (type)
    {
    case VERTEX:
      FLECTRON_LOG_DEBUG("Adding vertex shader source: {}", source->info());
      shaders.vertex = source;
      break;
    case FRAGMENT:
      FLECTRON_LOG_DEBUG("Adding fragment shader source: {}", source->info());
      shaders.fragment = source;
      break;
    case GEOMETRY:
      FLECTRON_LOG_DEBUG("Adding geometry shader source: {}", source->info());
      shaders.geometry = source;
      break;
    case COMPUTE:
      FLECTRON_LOG_DEBUG("Adding compute shader source: {}", source->info());
      shaders.compute = source;
      break;
    }

    GLuint shaderID = compileShader(type, source);
    glAttachShader(rendererID, shaderID);
    glLinkProgram(rendererID);
    glValidateProgram(rendererID);
    glDeleteShader(shaderID);
  }

  void Shader::reload()
  {
    FLECTRON_LOG_TRACE("Reloading shader");
    glDeleteProgram(rendererID);
    rendererID = glCreateProgram();

    ShadersAttacher attacher(rendererID, shaders);

    glLinkProgram(rendererID);
    glValidateProgram(rendererID);
  }

  Shader::ShadersAttacher::ShadersAttacher(GLuint rendererID, const Shaders& shaders)
    : vertex(0u), geometry(0u), fragment(0u), compute(0u)
  {
    if (shaders.vertex)
    {
      FLECTRON_LOG_DEBUG("\tVertex source: {}", shaders.vertex->info());
      vertex = compileShader(VERTEX, shaders.vertex);
      glAttachShader(rendererID, vertex);
    }
    
    if (shaders.geometry)
    {
      FLECTRON_LOG_DEBUG("\tGeometry source: {}", shaders.geometry->info());
      geometry = compileShader(GEOMETRY, shaders.geometry);
      glAttachShader(rendererID, geometry);
    }

    if (shaders.fragment)
    {
      FLECTRON_LOG_DEBUG("\tFragment source: {}", shaders.fragment->info());
      fragment = compileShader(FRAGMENT, shaders.fragment);
      glAttachShader(rendererID, fragment);
    }
    
    if (shaders.compute)
    {
      FLECTRON_LOG_DEBUG("\tCompute source: {}", shaders.compute->info());
      compute = compileShader(COMPUTE, shaders.compute);
      glAttachShader(rendererID, compute);
    }
  }

  Shader::ShadersAttacher::~ShadersAttacher()
  {
    glDeleteShader(vertex);
    glDeleteShader(geometry);
    glDeleteShader(fragment);
    glDeleteShader(compute);
  }

  Shader::WorkGoupInfo Shader::getWorkGroupInfo() const
  {
    WorkGoupInfo info;

    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &info.maxWorkGroupCount[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &info.maxWorkGroupCount[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &info.maxWorkGroupCount[2]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &info.maxWorkGroupSize[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &info.maxWorkGroupSize[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &info.maxWorkGroupSize[2]);
    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &info.maxWorkGroupInvocations);

    return info;
  }

  void Shader::dispatch(int x, int y, int z) const
  {
    glDispatchCompute((GLuint)x, (GLuint)y, (GLuint)z);
  }

  void Shader::barrier() const
  {
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  }

}
