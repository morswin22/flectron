#include <MindlessEngine/renderer.hpp>

#include <exception>
#include <fstream>
#include <iostream>

#include <MindlessEngine/body.hpp>

namespace MindlessEngine
{

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

  static const std::size_t MaxTriangleCount = 10000;
  static const std::size_t MaxVertexCount = MaxTriangleCount * 3;
  static const std::size_t MaxIndexCount = MaxTriangleCount * 3;

  struct Vertex
  {
    glm::vec2 position;
    glm::vec4 color;
  };

  struct RendererData
  {
    GLuint va = 0;
    GLuint vb = 0;
    GLuint ib = 0;

    Vertex* buffer = nullptr;
    Vertex* bufferPointer = nullptr;

    uint32_t* indices = nullptr;
    uint32_t* indicesPointer = nullptr;

    uint32_t indexCount = 0;
    uint32_t offset = 0;
  };

  static RendererData rendererData;

  void Renderer::init()
  {
    rendererData.buffer = new Vertex[MaxVertexCount];

    glCreateVertexArrays(1, &rendererData.va);
    glBindVertexArray(rendererData.va);

    glCreateBuffers(1, &rendererData.vb);
    glBindBuffer(GL_ARRAY_BUFFER, rendererData.vb);
    glBufferData(GL_ARRAY_BUFFER, MaxVertexCount * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexArrayAttrib(rendererData.va, 0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));

    glEnableVertexArrayAttrib(rendererData.va, 1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, color));

    rendererData.indices = new uint32_t[MaxIndexCount];

    glGenBuffers(1, &rendererData.ib);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererData.ib);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, MaxIndexCount * sizeof(uint32_t), nullptr, GL_DYNAMIC_DRAW);
  }

  void Renderer::shutdown()
  {
    glDeleteVertexArrays(1, &rendererData.va);
    glDeleteBuffers(1, &rendererData.vb);
    glDeleteBuffers(1, &rendererData.ib);

    delete[] rendererData.buffer;
    delete[] rendererData.indices;
  }

  void Renderer::beginBatch()
  {
    rendererData.bufferPointer = rendererData.buffer;
    rendererData.indicesPointer = rendererData.indices;
  }

  void Renderer::endBatch()
  {
    GLsizeiptr quadSize = (uint8_t*)rendererData.bufferPointer - (uint8_t*)rendererData.buffer;
    glBindBuffer(GL_ARRAY_BUFFER, rendererData.vb);
    glBufferSubData(GL_ARRAY_BUFFER, 0, quadSize, rendererData.buffer);

    GLsizeiptr indexSize = (uint8_t*)rendererData.indicesPointer - (uint8_t*)rendererData.indices;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererData.ib);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indexSize, rendererData.indices);
  }

  void Renderer::flush()
  {
    glBindVertexArray(rendererData.va);
    glBindBuffer(GL_ARRAY_BUFFER, rendererData.vb);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererData.ib);
    glDrawElements(GL_TRIANGLES, rendererData.indexCount, GL_UNSIGNED_INT, nullptr);

    rendererData.indexCount = 0;
    rendererData.offset = 0;
  }

  void Renderer::draw(const Vector* vertices, int numVertices, const uint32_t* triangles, const Color& color)
  {
    int numTriangles = (numVertices - 2) * 3;

    if (rendererData.indexCount + numTriangles >= MaxIndexCount)
    {
      endBatch();
      flush();
      beginBatch();
    }

    for (int i = 0; i < numVertices; i++)
    {
      rendererData.bufferPointer->position = { vertices[i].x, vertices[i].y };
      rendererData.bufferPointer->color = { color.r, color.g, color.b, color.a };
      rendererData.bufferPointer++;
    }

    for (int i = 0; i < numTriangles; i++)
    {
      rendererData.indices[rendererData.indexCount++] = triangles[i] + rendererData.offset;
      rendererData.indicesPointer++;
    }
    rendererData.offset += numVertices;
  
  }

};
