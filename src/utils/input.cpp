#include <flectron/utils/input.hpp>

namespace flectron
{

  std::array<bool, GLFW_KEY_LAST> Keyboard::pressed;
  std::array<bool, GLFW_KEY_LAST> Keyboard::clicked;

  void Keyboard::init(GLFWwindow* window)
  {
    pressed.fill(false);
    clicked.fill(false);
    glfwSetKeyCallback(window, keyboardCallback);
  }

  void Keyboard::reset()
  {
    clicked.fill(false);
  }

  bool Keyboard::isPressed(Key::Keys key)
  {
    return pressed[key];
  }

  bool Keyboard::isClicked(Key::Keys key)
  {
    return clicked[key];
  }

  void keyboardCallback(GLFWwindow*, int key, int, int action, int)
  {
    Keyboard::pressed[key] = action == GLFW_PRESS || action == GLFW_REPEAT;
    if (action == GLFW_PRESS)
      Keyboard::clicked[key] = true;
  }

  std::array<bool, GLFW_MOUSE_BUTTON_LAST> Mouse::pressed;
  std::array<bool, GLFW_MOUSE_BUTTON_LAST> Mouse::clicked;
  Vector Mouse::scroll;

  void Mouse::init(GLFWwindow* window)
  {
    pressed.fill(false);
    clicked.fill(false);
    glfwSetMouseButtonCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
  }

  void Mouse::reset()
  {
    clicked.fill(false);
    scroll.x = 0.0f;
    scroll.y = 0.0f;
  }

  bool Mouse::isPressed(Button::Buttons button)
  {
    return pressed[button];
  }

  bool Mouse::isClicked(Button::Buttons button)
  {
    return clicked[button];
  }

  void mouseCallback(GLFWwindow*, int button, int action, int)
  {
    Mouse::pressed[button] = action == GLFW_PRESS;
    if (action == GLFW_PRESS)
      Mouse::clicked[button] = true;
  }

  void scrollCallback(GLFWwindow*, double xoffset, double yoffset)
  {
    Mouse::scroll.x += static_cast<float>(xoffset);
    Mouse::scroll.y += static_cast<float>(yoffset);
  }

}
