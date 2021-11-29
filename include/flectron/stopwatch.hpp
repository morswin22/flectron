#pragma once

namespace flectron
{

  class Stopwatch
  {
  private:
    float startTime;
    float elapsedTime;

  public:
    Stopwatch();

    void start();
    void stop();
    float getElapsedTime() const;
  };
  
}
