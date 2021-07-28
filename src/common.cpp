#include "Mindless/common.hpp"

using namespace Mindless;

int Mindless::windowWidth{ 640 };
int Mindless::windowHeight{ 480 };
float Mindless::windowRatio{ (float)windowWidth / (float)windowHeight };

GLFWwindow* Mindless::window{ nullptr };
int Mindless::frameRate{ 60 };

glm::vec2 Mindless::mousePosition{ 0.0f, 0.0f };
bool Mindless::mousePressed[5]{ false };

bool Mindless::keyPressed[128]{ false };

bool Mindless::init()
{
  if (!glfwInit())
    return false;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

  window = glfwCreateWindow(windowWidth, windowHeight, "Loading", NULL, NULL);
  if (!window)
  {
    glfwTerminate();
    return false;
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  if (glewInit() != GLEW_OK)
    return false;

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glfwSetKeyCallback(window, keyCallback);
  glfwSetMouseButtonCallback(window, mouseCallback);

  return true;
}

void Mindless::loop()
{
  double mouseX, mouseY;

  while (!glfwWindowShouldClose(window))
  {
    glfwGetCursorPos(window, &mouseX, &mouseY);
    mousePosition.x = (float) mouseX;
    mousePosition.y = (float) mouseY;

    glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
    windowRatio = windowWidth / (float) windowHeight;

    glViewport(0, 0, windowWidth, windowHeight);
    glClear(GL_COLOR_BUFFER_BIT);

    draw();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
}

void Mindless::destroy()
{
  glfwDestroyWindow(window);
  glfwTerminate();
}

void Mindless::setTitle(const std::string& title)
{
  glfwSetWindowTitle(window, title.c_str());
}

bool Mindless::isMousePressed(int button)
{
  return mousePressed[button];
}

bool Mindless::isKeyPressed(int key)
{
  return keyPressed[key];
}

void Mindless::mouseCallback(GLFWwindow* window, int button, int action, int mods)
{
  mousePressed[button] = (action == GLFW_PRESS);
}

void Mindless::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  keyPressed[key] = (action == GLFW_PRESS);
}
