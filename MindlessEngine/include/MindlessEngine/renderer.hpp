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

  class VertexBuffer
  {
  private:
    GLuint rendererID;

  public:
    VertexBuffer(const void* data, unsigned int size);
    ~VertexBuffer();

    void bind() const;
    void unbind() const;
  };

  struct VertexBufferElement
  {
    unsigned int type;
    unsigned int count;
    unsigned char normalized;

    static unsigned int getSizeOfType(unsigned int type);
  };

  class VertexBufferLayout
  {
  public:
    std::vector<VertexBufferElement> elements;
    unsigned int stride;

    VertexBufferLayout();

    void pushFloat(unsigned int count);
    void pushUnsignedInt(unsigned int count);
    void pushUnsignedChar(unsigned int count);
  };

  class VertexArray
  {
  private:
    unsigned int rendererID;

  public:
    VertexArray();
    ~VertexArray();

    void addBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);

    void bind() const;
    void unbind() const;
  };

  class IndexBuffer
  {
  private:
    GLuint rendererID;
    unsigned int numIndices;
  
  public:
    IndexBuffer(unsigned int* indices, unsigned int numIndices);
    ~IndexBuffer();

    void bind() const;
    void unbind() const;

    friend void draw(const VertexArray& va, const IndexBuffer& ib);
  };

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

  void draw(const VertexArray& va, const IndexBuffer& ib);

};