#include <flectron/utils/input.hpp>

#include <iostream>

namespace flectron
{

  bool Keyboard::keys[128]{ false };

  bool Keyboard::isPressed(int key)
  {
    return keys[key];
  }

  void keyboardCallback(GLFWwindow*, int key, int, int action, int)
  {
    Keyboard::keys[key] = action == GLFW_PRESS || action == GLFW_REPEAT;
  }

  bool Mouse::buttons[9]{ false };
  float Mouse::scrollX{ 0.0f };
  float Mouse::scrollY{ 0.0f };

  bool Mouse::isPressed(int button)
  {
    return buttons[button];
  }

  float Mouse::getScrollX()
  {
    return scrollX;
  }

  float Mouse::getScrollY()
  {
    return scrollY;
  }

  void Mouse::resetScroll()
  {
    scrollX = 0.0f;
    scrollY = 0.0f;
  }

  void mouseCallback(GLFWwindow*, int button, int action, int)
  {
    Mouse::buttons[button] = action == GLFW_PRESS;
  }

  void scrollCallback(GLFWwindow*, double xoffset, double yoffset)
  {
    Mouse::scrollX += (float)xoffset;
    Mouse::scrollY += (float)yoffset;
  }

}
