#include <flectron/utils/stopwatch.hpp>

#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace flectron
{

  Stopwatch::Stopwatch()
    : startTime(0.0f), elapsedTime(0.0f)
  {}

  void Stopwatch::start()
  {
    startTime = glfwGetTime();
  }

  void Stopwatch::stop()
  {
    elapsedTime = glfwGetTime() - startTime;
  }

  float Stopwatch::getElapsedTime() const
  {
    return elapsedTime;
  }

}
