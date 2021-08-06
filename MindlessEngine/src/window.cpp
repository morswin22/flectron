#include <MindlessEngine/window.hpp>

#include <MindlessEngine/input.hpp>
#include <MindlessEngine/math.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace MindlessEngine
{

  void GLAPIENTRY
  GLMessageCallback( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam )
  {
    if ( type == GL_DEBUG_TYPE_ERROR ) fprintf( stderr, "GL CALLBACK: type = 0x%x, severity = 0x%x, message = %s\n", type, severity, message );
  }
  
  Window::Window(int width, int height, const std::string& title) 
  : window(nullptr), width(width), height(height), title(title), 
    desiredFrameRate(60.0f), desiredInterval(1.0f / desiredFrameRate), lastMeasuredTime(0), 
    cameraPosition(glm::vec3(width * 0.5f, height * 0.5f, 0.0f)), 
    cameraScale(glm::vec3(1.0f, 1.0f, 1.0f)),
    projection(glm::ortho(0.0f, (float)width, 0.0f, (float)height, -1.0f, 1.0f)), 
    view(glm::scale(glm::translate(glm::mat4(1.0f), cameraPosition), cameraScale)),
    lineVertexArray(nullptr), lineVertexLayout(), lineIndexBuffer(nullptr), strokeWeight(1.0f)
  {
    if (!glfwInit())
      return;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window)
    {
      glfwTerminate();
      return;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

    if (glewInit() != GLEW_OK)
      return;

    glEnable(GL_PROGRAM_POINT_SIZE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(GLMessageCallback, 0);

    glfwSetKeyCallback(window, keyboardCallback);
    glfwSetMouseButtonCallback(window, mouseCallback);
    
    lineVertexArray = std::make_unique<VertexArray>();
    lineVertexLayout.pushFloat(2);

    unsigned int lineIndices[]{
      0, 1, 2,
      2, 3, 0
    };
    lineIndexBuffer = std::make_unique<IndexBuffer>(lineIndices, 6);
  }

  Window::~Window()
  {
    glfwDestroyWindow(window);
    glfwTerminate();
  }

  void Window::setTitle(const std::string& title)
  {
    this->title = title;
    glfwSetWindowTitle(window, title.c_str());
  }

  std::string Window::getTitle() const
  {
    return title;
  }

  bool Window::shouldClose() const
  {
    return glfwWindowShouldClose(window);
  }
  void Window::getCursorPosition(Vector& v) const
  {
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    v.x = (float) x;
    v.y = (float) y;
  }

  void Window::getFrameSize()
  {
    glfwGetFramebufferSize(window, &width, &height);
    // windowRatio = width / (float) height;
    glViewport(0, 0, width, height);
    projection = glm::ortho(0.0f, (float)width, 0.0f, (float)height, -1.0f, 1.0f);
  }

  void Window::clear() const
  {
    glClear(GL_COLOR_BUFFER_BIT);
  }

  void Window::swapBuffers() const
  {
    glfwSwapBuffers(window);
  }

  void Window::pollEvents() const
  {
    glfwPollEvents();
  }

  float Window::getElapsedTime()
  {
    while ((float) glfwGetTime() - lastMeasuredTime < desiredInterval && !shouldClose())
      pollEvents();

    float temp = lastMeasuredTime;
    lastMeasuredTime = (float) glfwGetTime();
    return lastMeasuredTime - temp;
  }

  glm::mat4 Window::getProjectionMatrix() const
  {
    return projection * view;
  }

  void Window::getCameraConstrains(float* left, float* top, float* right, float* bottom) const
  {
    *left = -cameraPosition.x / cameraScale.x;
    *top = cameraPosition.y / cameraScale.y;
    *right = cameraPosition.x / cameraScale.x;
    *bottom = -cameraPosition.y / cameraScale.y;
  }

  void Window::setScale(float x, float y)
  { 
    cameraScale.x = x;
    cameraScale.y = y;
    view = glm::scale(glm::translate(glm::mat4(1.0f), cameraPosition), cameraScale);
  }

  void Window::setScale(float a)
  {
    cameraScale.x = a;
    cameraScale.y = a;
    view = glm::scale(glm::translate(glm::mat4(1.0f), cameraPosition), cameraScale);
  }

  void Window::setSize(int width, int height)
  {
    glfwSetWindowSize(window, width, height);
    this->width = width;
    this->height = height;
    projection = glm::ortho(0.0f, (float)width, 0.0f, (float)height, -1.0f, 1.0f);
  }

  void Window::setBackground(const Color& color)
  {
    glClearColor(color.r, color.g, color.b, color.a);
  }

  void Window::setDesiredFrameRate(float desiredFrameRate)
  {
    this->desiredFrameRate = desiredFrameRate;
    desiredInterval = 1.0f / desiredFrameRate;
  }

  float Window::getDesiredFrameRate() const
  {
    return desiredFrameRate;
  }

  void Window::draw(const VertexArray& va, const IndexBuffer& ib)
  {
    va.bind();
    ib.bind();
    glDrawElements(GL_TRIANGLES, ib.numIndices, GL_UNSIGNED_INT, 0);
  }

  void Window::draw(Body& body)
  {
    body.getVertexArray()->bind();
    body.getIndexBuffer()->bind();
    glDrawElements(GL_TRIANGLES, (body.getNumVertices() - 2) * 3, GL_UNSIGNED_INT, 0);
  }

  void Window::drawLine(const Vector& a, const Vector& b)
  {
    Vector line = b - a;

    Vector invCameraScale( 1.0f / cameraScale.x, 1.0f / cameraScale.y );
    Vector axis = normalize(line) * strokeWeight;
    Vector normal = normalize({ -axis.y, axis.x  }) * strokeWeight;

    axis.x *= invCameraScale.x;
    axis.y *= invCameraScale.y;

    normal.x *= invCameraScale.x;
    normal.y *= invCameraScale.y;

    Vector p0 = a + normal - axis;
    Vector p1 = a - normal - axis;

    Vector p2 = b - normal + axis;
    Vector p3 = b + normal + axis;

    float positions[]{
      p0.x, p0.y,
      p1.x, p1.y,
      p2.x, p2.y,
      p3.x, p3.y
    };

    VertexBuffer vb(positions, 8 * sizeof(float));
    lineVertexArray->addBuffer(vb, lineVertexLayout);

    lineVertexArray->bind();
    lineIndexBuffer->bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  }

};
