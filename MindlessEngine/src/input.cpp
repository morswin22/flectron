#include <MindlessEngine/input.hpp>

#include <iostream>

namespace MindlessEngine
{

  bool Keyboard::keys[128]{ false };

  bool Keyboard::isPressed(int key)
  {
    return keys[key];
  }

  void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
  {
    Keyboard::keys[key] = action == GLFW_PRESS || action == GLFW_REPEAT;
  }

  bool Mouse::buttons[9]{ false };

  bool Mouse::isPressed(int button)
  {
    return buttons[button];
  }

  void mouseCallback(GLFWwindow* window, int button, int action, int mods)
  {
    Mouse::buttons[button] = action == GLFW_PRESS;
  }

};