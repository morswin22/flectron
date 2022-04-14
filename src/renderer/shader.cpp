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

  std::string BaseShader::getSource(const std::string& filepath) const
  {
    std::ifstream file(filepath);

    FLECTRON_ASSERT(file.is_open(), "Could not open file " + filepath);
    
    std::string source;
    std::string line;
    while (std::getline(file, line))
      source += line + "\n";
    
    return source;
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

  Shader::Shader(const std::string& filepathVertex, const std::string& filepathFragment)
    : filepathVertex(filepathVertex), filepathFragment(filepathFragment)
  {
    FLECTRON_LOG_TRACE("Creating shader from {} and {}", filepathVertex, filepathFragment);

    std::string vertexShader = getSource(filepathVertex);
    std::string fragmentShader = getSource(filepathFragment);

    rendererID = glCreateProgram();
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(rendererID, vs);
    glAttachShader(rendererID, fs);
    glLinkProgram(rendererID);
    glValidateProgram(rendererID);

    glDeleteShader(vs);
    glDeleteShader(fs);
  }

  ComputeShader::ComputeShader(const std::string& filepath)
    : filepath(filepath)
  {
    FLECTRON_LOG_TRACE("Creating compute shader from {}", filepath);

    std::string source = getSource(filepath);

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
