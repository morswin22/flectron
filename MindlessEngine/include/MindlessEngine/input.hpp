#pragma once

#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace MindlessEngine
{

  enum Keys
  {
    SPACE = 32, APOSTROPHE = 39, COMMA = 44, MINUS = 45, PERIOD = 46, SLASH = 47, NUM_0 = 48, NUM_1 = 49, 
    NUM_2 = 50, NUM_3 = 51, NUM_4 = 52, NUM_5 = 53, NUM_6 = 54, NUM_7 = 55, NUM_8 = 56, NUM_9 = 57, 
    SEMICOLON = 59, EQUAL = 61, A = 65, B = 66, C = 67, D = 68, E = 69, F = 70, G = 71, H = 72, I = 73, 
    J = 74, K = 75, L = 76, M = 77, N = 78, O = 79, P = 80, Q = 81, R = 82, S = 83, T = 84, U = 85, V = 86,
    W = 87, X = 88, Y = 89, Z = 90 
  };

  enum Buttons
  {
    LEFT = 0, RIGHT = 1, MIDDLE = 2
  };

  class Keyboard 
  {
  private:
    static bool keys[128];

  public:
    static bool isPressed(int key);

    friend void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
  };


  class Mouse
  {
  private:
    static bool buttons[9];
    static float scrollX;
    static float scrollY;

  public:
    static bool isPressed(int button);

    static float getScrollX();
    static float getScrollY();

    static void resetScroll();

    friend void mouseCallback(GLFWwindow* window, int button, int action, int mods);
    friend void scrollCallback(GLFWwindow* window, double x, double y);
  };

  void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
  void mouseCallback(GLFWwindow* window, int button, int action, int mods);
  void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

};
