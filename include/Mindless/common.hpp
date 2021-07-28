#ifndef COMMON_HPP
#define COMMON_HPP

#include "GL/glew.h"
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

#include <iostream>

namespace Mindless
{

  extern int windowWidth;
  extern int windowHeight;
  extern float windowRatio;

  extern glm::vec2 mousePosition;
  extern bool mousePressed[5];

  extern bool keyPressed[128];

  extern int frameRate;
  extern int actualFrameRate;

  extern GLFWwindow* window;

  bool init();
  void loop();
  void destroy();

  void draw();

  void setTitle(const std::string& title);

  bool isMousePressed(int button);
  bool isKeyPressed(int key);

  void mouseCallback(GLFWwindow* window, int button, int action, int mods);
  void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

};

#endif