#include <flectron/renderer/shader.hpp>
#include <flectron/assert/assert.hpp>

#include <sstream>
#include <fstream>
#include <string>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

namespace flectron 
{

  BaseShader::~BaseShader()
  {
    glDeleteProgram(rendererID);
  }

  void BaseShader::bind() const
  {
    glUseProgram(rendererID);
  }

  void BaseShader::unbind() const
  {
    glUseProgram(0);
  }

  GLuint BaseShader::getRendererID() const
  {
    return rendererID;
  }

  void BaseShader::setUniform1i(const std::string& name, int value)
  {
    glUniform1i(getUniformLocation(name), value);
  }

  void BaseShader::setUniform1f(const std::string& name, float value)
  {
    glUniform1f(getUniformLocation(name), value);
  }
  
  void BaseShader::setUniform2f(const std::string& name, float v1, float v2)
  {
    glUniform2f(getUniformLocation(name), v1, v2);
  }

  void BaseShader::setUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
  {
    glUniform4f(getUniformLocation(name), v0, v1, v2, v3);
  }

  void BaseShader::setUniformMat4f(const std::string& name, const glm::mat4& matrix)
  {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
  }

  void BaseShader::setUniform1iv(const std::string& name, int* array, int size)
  {
    glUniform1iv(getUniformLocation(name), size, array);
  }

  void BaseShader::setUniform2fv(const std::string& name, float* array, int size)
  {
    glUniform2fv(getUniformLocation(name), size, array);
  }

  void BaseShader::setUniform3fv(const std::string& name, float* array, int size)
  {
    glUniform3fv(getUniformLocation(name), size, array);
  }

  void BaseShader::setUniform4fv(const std::string& name, float* array, int size)
  {
    glUniform4fv(getUniformLocation(name), size, array);
  }

  int BaseShader::getUniformLocation(const std::string& name)
  {
    if (locationCache.find(name) != locationCache.end())
      return locationCache[name];

    int location = glGetUniformLocation(this->rendererID, name.c_str());
    FLECTRON_ASSERT(location != -1, "Uniform " + name + " not found");
    
    locationCache[name] = location;
    return location;
  }

  unsigned int BaseShader::compileShader(unsigned int type, const std::string& source)
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

  Shader::Shader(const TextView& vertexSource, const TextView& fragmentSource)
    : vertexSource(vertexSource), fragmentSource(fragmentSource)
  {
    FLECTRON_LOG_TRACE("Creating shader");
    FLECTRON_LOG_DEBUG("\tVertex source: {}", vertexSource->info());
    FLECTRON_LOG_DEBUG("\tFramgent source: {}", fragmentSource->info());

    rendererID = glCreateProgram();
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexSource);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentSource);

    glAttachShader(rendererID, vs);
    glAttachShader(rendererID, fs);
    glLinkProgram(rendererID);
    glValidateProgram(rendererID);

    glDeleteShader(vs);
    glDeleteShader(fs);
  }

  ComputeShader::ComputeShader(const TextView& source)
    : source(source)
  {
    FLECTRON_LOG_TRACE("Creating compute shader");
    FLECTRON_LOG_DEBUG("\tSource: {}", source->info());

    rendererID = glCreateProgram();
    unsigned int cs = compileShader(GL_COMPUTE_SHADER, source);

    glAttachShader(rendererID, cs);
    glLinkProgram(rendererID);
    glValidateProgram(rendererID);

    glDeleteShader(cs);

    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &maxWorkGroupCount[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &maxWorkGroupCount[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &maxWorkGroupCount[2]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &maxWorkGroupSize[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &maxWorkGroupSize[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &maxWorkGroupSize[2]);
    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &maxWorkGroupInvocations);
  }

  void ComputeShader::dispatch(int x, int y, int z) const
  {
    glDispatchCompute((GLuint)x, (GLuint)y, (GLuint)z);
  }

  void ComputeShader::barrier() const
  {
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  }

}
