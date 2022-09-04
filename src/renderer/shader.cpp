#include <flectron/renderer/shader.hpp>
#include <flectron/assert/assert.hpp>

#include <sstream>
#include <fstream>
#include <string>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

namespace flectron 
{

  GLint Shader::maxUniformBlockBinings = 0;
  Shader::WorkGroupInfo Shader::workGroupInfo = { 0, 0, 0, 0, 0, 0, 0 };

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

  void Shader::setUniformBlock(const std::string& name, GLuint ubo)
  {
    GLuint binding = getUniformBlockBinding(name);
    glBindBufferBase(GL_UNIFORM_BUFFER, binding, ubo);
    glUniformBlockBinding(rendererID, getUniformBlockIndex(name), binding);
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

  int Shader::getUniformBlockIndex(const std::string& name)
  {
    if (locationCache.find(name) != locationCache.end())
      return locationCache[name];

    int location = glGetUniformBlockIndex(this->rendererID, name.c_str());
    FLECTRON_ASSERT(location != -1, "Uniform block " + name + " not found");

    locationCache[name] = location;
    return location;
  }

  GLuint Shader::getUniformBlockBinding(const std::string& name)
  {
    if (uniformBlockBindings.find(name) != uniformBlockBindings.end())
      return uniformBlockBindings[name];

    GLuint binding = uniformBlockBindings.size();
    FLECTRON_ASSERT(binding < maxUniformBlockBinings, "Maximum number of uniform block bindings reached");
    
    uniformBlockBindings[name] = binding;
    return binding;
  }

  GLint Shader::getMaxUniformBlockBinings()
  {
    GLint max;
    glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &max);
    return max;
  }

  unsigned int Shader::compileShader(unsigned int type, const std::string& source)
  {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    switch (type)
    {
    case VERTEX:
      checkShaderError(id, GL_COMPILE_STATUS, "Failed to compile vertex shader! {}");
      break;
    case FRAGMENT:
      checkShaderError(id, GL_COMPILE_STATUS, "Failed to compile fragment shader! {}");
      break;
    case GEOMETRY:
      checkShaderError(id, GL_COMPILE_STATUS, "Failed to compile geometry shader! {}");
      break;
    case COMPUTE:
      checkShaderError(id, GL_COMPILE_STATUS, "Failed to compile compute shader! {}");
      break;
    }

    return id;
  }

  void Shader::checkShaderError(GLuint rendererID, GLenum error, const std::string& format)
  {
    int success;
    glGetShaderiv(rendererID, error, &success);
    if (success == GL_FALSE)
    {
      int length;
      glGetShaderiv(rendererID, GL_INFO_LOG_LENGTH, &length);
      char* message = (char*)alloca(length * sizeof(char));
      glGetShaderInfoLog(rendererID, length, &length, message);
      glDeleteShader(rendererID);
      FLECTRON_ASSERT(false, fmt::format(format, message));
    }
  }

  void Shader::checkProgramError(GLuint rendererID, GLenum error, const std::string& format)
  {
    int success;
    glGetProgramiv(rendererID, error, &success);
    if (success == GL_FALSE)
    {
      int length;
      glGetProgramiv(rendererID, GL_INFO_LOG_LENGTH, &length);
      char* message = (char*)alloca(length * sizeof(char));
      glGetProgramInfoLog(rendererID, length, &length, message);
      glDeleteProgram(rendererID);
      FLECTRON_ASSERT(false, fmt::format(format, message));
    }
  }

  Shader::Shader()
    : rendererID(0u), locationCache(), shaders({ nullptr, nullptr, nullptr, nullptr })
  {
    FLECTRON_LOG_TRACE("Creating empty shader");
    rendererID = glCreateProgram();
    linkAndValidate();
  }

  Shader::Shader(const Shaders& shaders)
    : rendererID(0u), locationCache(), shaders(shaders)
  {
    FLECTRON_LOG_TRACE("Creating shader");
    rendererID = glCreateProgram();

    ShadersAttacher attacher(rendererID, shaders);

    linkAndValidate();
  }

  void Shader::init()
  {
    maxUniformBlockBinings = getMaxUniformBlockBinings();
    workGroupInfo = getWorkGroupInfo();
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
    linkAndValidate();
    glDeleteShader(shaderID);
  }

  void Shader::reload()
  {
    FLECTRON_LOG_TRACE("Reloading shader");
    glDeleteProgram(rendererID);
    rendererID = glCreateProgram();

    ShadersAttacher attacher(rendererID, shaders);

    linkAndValidate();
  }

  void Shader::resetUniformBlockBindings()
  {
    uniformBlockBindings.clear();
  }

  void Shader::linkAndValidate()
  {
    glLinkProgram(rendererID);
    checkProgramError(rendererID, GL_LINK_STATUS, "Failed to link shader program! {}");

    glValidateProgram(rendererID);
    checkProgramError(rendererID, GL_VALIDATE_STATUS, "Failed to validate shader program! {}");
  }

  Shader::ShadersAttacher::ShadersAttacher(GLuint rendererID, const Shaders& shaders)
    : rendererID(rendererID), vertex(0u), geometry(0u), fragment(0u), compute(0u)
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
    glDetachShader(rendererID, vertex);
    glDeleteShader(vertex);
    glDetachShader(rendererID, geometry);
    glDeleteShader(geometry);
    glDetachShader(rendererID, fragment);
    glDeleteShader(fragment);
    glDetachShader(rendererID, compute);
    glDeleteShader(compute);
  }

  Shader::WorkGroupInfo Shader::getWorkGroupInfo()
  {
    WorkGroupInfo info;

    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &info.maxCount[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &info.maxCount[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &info.maxCount[2]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &info.maxSize[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &info.maxSize[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &info.maxSize[2]);
    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &info.maxInvocations);

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
