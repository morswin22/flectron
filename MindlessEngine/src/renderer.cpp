#include <MindlessEngine/renderer.hpp>

#include <exception>
#include <fstream>
#include <iostream>

#include <MindlessEngine/body.hpp>

namespace MindlessEngine
{

  VertexBuffer::VertexBuffer(const void* data, unsigned int size)
  {
    glGenBuffers(1, &rendererID);
    glBindBuffer(GL_ARRAY_BUFFER, rendererID);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
  }

  VertexBuffer::~VertexBuffer()
  {
    glDeleteBuffers(1, &rendererID);
  }

  void VertexBuffer::bind() const
  {
    glBindBuffer(GL_ARRAY_BUFFER, rendererID);
  }

  void VertexBuffer::unbind() const
  {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

  unsigned int VertexBufferElement::getSizeOfType(unsigned int type)
  {
    switch (type)
    {
      case GL_FLOAT:         return 4;
      case GL_UNSIGNED_INT:  return 4;
      case GL_UNSIGNED_BYTE: return 1;
    }
    return 0;
  }

  VertexBufferLayout::VertexBufferLayout() : elements(), stride(0) {}

  void VertexBufferLayout::pushFloat(unsigned int count)
  {
    elements.push_back({ GL_FLOAT, count, GL_FALSE });
    stride += VertexBufferElement::getSizeOfType(GL_FLOAT) * count;
  }

  void VertexBufferLayout::pushUnsignedInt(unsigned int count)
  {
    elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
    stride += VertexBufferElement::getSizeOfType(GL_UNSIGNED_INT) * count;
  }

  void VertexBufferLayout::pushUnsignedChar(unsigned int count)
  {
    elements.push_back({ GL_UNSIGNED_BYTE, count, GL_FALSE });
    stride += VertexBufferElement::getSizeOfType(GL_UNSIGNED_BYTE) * count;
  }

  VertexArray::VertexArray()
  {
    glGenVertexArrays(1, &rendererID);
  }

  VertexArray::~VertexArray()
  {
    glDeleteVertexArrays(1, &rendererID);
  }

  void VertexArray::addBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
  {
    bind();
    vb.bind();
    unsigned int offset = 0;
    for (unsigned int i = 0; i < layout.elements.size(); i++)
    {
      const VertexBufferElement& element = layout.elements[i];
      glEnableVertexAttribArray(i);
      glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.stride, (const void*) offset);
      offset += element.count * VertexBufferElement::getSizeOfType(element.type);
    }
  }

  void VertexArray::bind() const
  {
    glBindVertexArray(rendererID);
  }

  void VertexArray::unbind() const
  {
    glBindVertexArray(0);
  }

  IndexBuffer::IndexBuffer(unsigned int* indices, unsigned int numIndices) : numIndices(numIndices)
  {
    glGenBuffers(1, &rendererID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(unsigned int), indices, GL_STATIC_DRAW);
  }

  IndexBuffer::~IndexBuffer()
  {
    glDeleteBuffers(1, &rendererID);
  }

  void IndexBuffer::bind() const
  {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererID);
  }

  void IndexBuffer::unbind() const
  {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }

  Shader::Shader(const std::string& filepathVertex, const std::string& filepathFragment)
    : filepathVertex(filepathVertex), filepathFragment(filepathFragment)
  {
    std::string vertexShader = getSource(filepathVertex);
    std::string fragmentShader = getSource(filepathFragment);

    rendererID = createShader(vertexShader, fragmentShader);
  }

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

  void Shader::setUniform1i(const std::string& name, int value)
  {
    glUniform1i(getUniformLocation(name), value);
  }

  void Shader::setUniform1f(const std::string& name, float value)
  {
    glUniform1f(getUniformLocation(name), value);
  }

  void Shader::setUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
  {
    glUniform4f(getUniformLocation(name), v0, v1, v2, v3);
  }

  void Shader::setUniformMat4f(const std::string& name, const glm::mat4& matrix)
  {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
  }

  int Shader::getUniformLocation(const std::string& name)
  {
    if (locationCache.find(name) != locationCache.end())
      return locationCache[name];

    int location = glGetUniformLocation(this->rendererID, name.c_str());
    if (location == -1)
      std::runtime_error("Warning: uniform " + name + " doesn't exist!\n");
    
    locationCache[name] = location;
    return location;
  }

  std::string Shader::getSource(const std::string& filepath) const
  {
    std::ifstream file(filepath);

    if (!file.is_open())
      throw std::runtime_error("Error: could not open file " + filepath + "\n");
    
    std::string source;
    std::string line;
    while (std::getline(file, line))
      source += line + "\n";
    
    return source;
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
      std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!\n";
      std::cout << message << "\n";
      glDeleteShader(id);
      return 0;
    }

    return id;
  }

  unsigned int Shader::createShader(const std::string& vertexShader, const std::string& fragmentShader)
  {
    unsigned int program = glCreateProgram();
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
  }

  void draw(const VertexArray& va, const IndexBuffer& ib)
  {
    va.bind();
    ib.bind();
    glDrawElements(GL_TRIANGLES, ib.numIndices, GL_UNSIGNED_INT, 0);
  }

  void draw(Body& body)
  {
    body.getVertexArray()->bind();
    body.getIndexBuffer()->bind();
    glDrawElements(GL_TRIANGLES, (body.getNumVertices() - 2) * 3, GL_UNSIGNED_INT, 0);
  }

};
