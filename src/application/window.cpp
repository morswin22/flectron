#include <flectron/application/window.hpp>

#include <flectron/utils/input.hpp>
#include <flectron/physics/math.hpp>
#include <flectron/renderer/renderer.hpp>
#include <flectron/utils/profile.hpp>
#include <flectron/scene/entity.hpp>
#include <sstream>

namespace flectron
{

  void GLAPIENTRY
  GLMessageCallback( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei, const GLchar* message, const void* )
  {
    if (type == GL_DEBUG_TYPE_ERROR) fprintf(stderr, "GL CALLBACK: source = 0x%x, type = 0x%x, id = 0x%x, severity = 0x%x, message = %s\n", source, type, id, severity, message);
  }
  
  WindowProperties::WindowProperties(const std::string& title, WindowFlags flags)
    : title(title), width(800), height(600), fps(60.0f), fullscreen(flags & FULLSCREEN), vsync(flags & VSYNC)
  {
  }

  WindowProperties::WindowProperties(const std::string& title, int width, int height, WindowFlags flags)
    : title(title), width(width), height(height), fps(60.0f), fullscreen(false), vsync(flags & VSYNC)
  {
  }

  WindowProperties::WindowProperties(const std::string& title, int width, int height, float fps)
    : title(title), width(width), height(height), fps(fps), fullscreen(false), vsync(false)
  {
  }

  Window::Window(const WindowProperties& properties, const std::string& shaderVertPath, const std::string& shaderFragPath) 
  : properties(properties), window(nullptr),
    shader(nullptr),
    desiredInterval(1.0f / properties.fps), lastMeasuredTime(0), 
    camera(-properties.width * 0.5f, properties.width * 0.5f, -properties.height * 0.5f, properties.height * 0.5f)
  {
    if (!glfwInit())
      return;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    window = glfwCreateWindow(properties.width, properties.height, properties.title.c_str(), nullptr, nullptr);
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
    glfwSetScrollCallback(window, scrollCallback);

    shader = createRef<Shader>(shaderVertPath, shaderFragPath);
    Renderer::init(shader, properties.width, properties.height, rendererBuffer);
  }

  Window::~Window()
  {
    Renderer::shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();
  }

  void Window::setTitle(const std::string& title)
  {
    properties.title = title;
    glfwSetWindowTitle(window, title.c_str());
  }

  std::string Window::getTitle() const
  {
    return properties.title;
  }

  GLuint Window::getRendererBuffer() const
  {
    return rendererBuffer;
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
    int newWidth, newHeight;
    glfwGetFramebufferSize(window, &newWidth, &newHeight);
    if (newWidth == properties.width && newHeight == properties.height)
      return;

    properties.width = newWidth;
    properties.height = newHeight;
    glViewport(0, 0, properties.width, properties.height);
    const float scale = camera.getScale();
    camera.setProjection(-properties.width * 0.5f * scale, properties.width * 0.5f * scale, -properties.height * 0.5f * scale, properties.height * 0.5f * scale);
    glBindTexture(GL_TEXTURE_2D, rendererBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, properties.width, properties.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
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
    float temp = lastMeasuredTime;
    lastMeasuredTime = (float) glfwGetTime();
    return lastMeasuredTime - temp;
  }

  void Window::setSize(int width, int height)
  {
    glfwSetWindowSize(window, width, height);
    glViewport(0, 0, width, height);
    properties.width = width;
    properties.height = height;
    const float scale = camera.getScale();
    camera.setProjection(-width * 0.5f * scale, width * 0.5f * scale, -height * 0.5f * scale, height * 0.5f * scale);
    glBindTexture(GL_TEXTURE_2D, rendererBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  void Window::setBackground(const Color& color)
  {
    glClearColor(color.r, color.g, color.b, color.a);
  }

  void Window::setDesiredFrameRate(float desiredFrameRate)
  {
    properties.fps = desiredFrameRate;
    desiredInterval = 1.0f / properties.fps;
  }

  float Window::getDesiredFrameRate() const
  {
    return properties.fps;
  }

  void Window::regulateFrameRate()
  {
    FLECTRON_PROFILE_EVENT("Window::regulateFrameRate");
    while ((float) glfwGetTime() - lastMeasuredTime < desiredInterval && !shouldClose())
      pollEvents();
  }

  void Window::draw(Entity entity)
  {
    auto& pc = entity.get<PositionComponent>();
    auto& vc = entity.get<VertexComponent>();

    if (entity.has<StrokeComponent>())
    {
      auto& vertices = vc.getTransformedVertices(pc);
      auto& sc = entity.get<StrokeComponent>();
      for (int j = 0; j < vertices.size(); j++)
        draw(vertices[j], vertices[(j + 1) % vertices.size()], sc.strokeWidth, sc.strokeColor);
    }

    if (entity.has<AnimationComponent>())
    {
      auto& ac = entity.get<AnimationComponent>();
      glm::vec4* frame = ac.animationAtlas->getAnimation(ac.animationState.currentName)->getNext(ac.animationState);
      Color color = entity.has<FillComponent>() ? entity.get<FillComponent>().fillColor : Colors::white();
      if (entity.has<TextureVertexComponent>())
      {
        auto& tvc = entity.get<TextureVertexComponent>();
        auto& vertices = tvc.getTransformedVertices(pc);
        Renderer::draw(vertices[0], vertices[1], vertices[2], vertices[3], static_cast<Texture*>(ac.animationAtlas.get())->get(), *frame, color);
      }
      else if (entity.has<BoxComponent>())
      {
        auto& vertices = vc.getTransformedVertices(pc);
        Renderer::draw(vertices[0], vertices[1], vertices[2], vertices[3], static_cast<Texture*>(ac.animationAtlas.get())->get(), *frame, color);
      }
      else
      {
        throw std::runtime_error("Use TextureVertexComponent for non-box bodies");
      }
    }
    else if (entity.has<TextureComponent>())
    {
      auto& tc = entity.get<TextureComponent>();
      Color color = entity.has<FillComponent>() ? entity.get<FillComponent>().fillColor : Colors::white();
      if (entity.has<TextureVertexComponent>())
      {
        auto& tvc = entity.get<TextureVertexComponent>();
        auto& vertices = tvc.getTransformedVertices(pc);
        Renderer::draw(vertices[0], vertices[1], vertices[2], vertices[3], tc.textureIndex, tc.texturePositions, color);
      }
      else if (entity.has<BoxComponent>())
      {
        auto& vertices = vc.getTransformedVertices(pc);
        Renderer::draw(vertices[0], vertices[1], vertices[2], vertices[3], tc.textureIndex, tc.texturePositions, color);
      }
      else
      {
        throw std::runtime_error("Use TextureVertexComponent for non-box bodies");
      }
    }
    else if (entity.has<FillComponent>())
    {
      Renderer::draw(vc.getTransformedVertices(pc), vc.triangles, entity.get<FillComponent>().fillColor);
    }
  }

  void Window::draw(const AABB& aabb, const Color& color)
  {
    Vector a(aabb.min.x, aabb.min.y);
    Vector b(aabb.max.x, aabb.min.y);
    Vector c(aabb.max.x, aabb.max.y);
    Vector d(aabb.min.x, aabb.max.y);

    draw(a, b, 1.0, color);
    draw(b, c, 1.0, color);
    draw(c, d, 1.0, color);
    draw(d, a, 1.0, color);
  }

  void Window::draw(const Vector& a, const Vector& b, float weight, const Color& color)
  {
    Vector line = b - a;

    const float scale = camera.getScale();
    Vector axis = normalize(line) * weight * scale;
    Vector normal = normalize({ -axis.y, axis.x  }) * weight * scale;

    const std::vector<Vector> vertices{
      a + normal - axis,
      a - normal - axis,
      b - normal + axis,
      b + normal + axis
    };

    const std::vector<size_t> triangles{
      0, 1, 2,
      2, 3, 0
    };

    Renderer::draw(vertices, triangles, color);
  }

  void Window::draw(Ref<FontAtlas>& atlas, const Vector& position, const std::string& text, float scale, const Color& color)
  {
    std::stringstream ss(text);
    std::string line;

    if (text.size() == 0.0f)
      return;

    float lineOffset = 0.0f;

    while(std::getline(ss, line, '\n'))
    {
      glm::vec4* texturePositions = new glm::vec4[line.size()];
      GLuint texture = atlas->get(line, texturePositions);
      glm::vec2 offsets = atlas->getOffsets() * scale;

      for (float i = 0; i < line.size(); i++)
      {
        Renderer::draw(
          {position.x + i * offsets.x, position.y - lineOffset}, 
          {position.x + (i + 1.0f) * offsets.x, position.y - lineOffset}, 
          {position.x + (i + 1.0f) * offsets.x, position.y - offsets.y - lineOffset}, 
          {position.x + i * offsets.x, position.y - offsets.y - lineOffset}, 
          texture, 
          texturePositions[(int)i],
          color
        );
      }

      delete[] texturePositions;
      lineOffset += offsets.y;
    }
  }

  void Window::draw(const Scope<LightRenderer>& scene, const Color& nightColor, float darkness)
  {
    const float scale = camera.getScale();
    scene->render(
      nightColor,
      darkness,
      camera.getPosition(),
      { properties.width * scale, properties.height * scale },
      rendererBuffer
    );
  }

  void Window::draw(const Vector& position, float size, const WFC::Tile* tile, const Color& color)
  {
    Renderer::draw(
      {position.x + tile->ab.x * size, position.y + tile->ab.y * size},
      {position.x + tile->ab.z * size, position.y + tile->ab.w * size},
      {position.x + tile->cd.x * size, position.y + tile->cd.y * size},
      {position.x + tile->cd.z * size, position.y + tile->cd.w * size},
      tile->texture,
      tile->textureCoords,
      color
    );
  }

}
