#include <flectron/scene/datetime.hpp>
#include <flectron/scene/scene.hpp>
#include <flectron/physics/math.hpp>

namespace flectron
{

  DateTime::DateTime(float startTime, float nightToDay, float dayToNight, float transitionTime, float scale, float minDarkness, float maxDarkness)
    : time(startTime), scale(scale), transitionTime(transitionTime), minDarkness(minDarkness), maxDarkness(maxDarkness), day(0)
  {
    timeZones[0] = nightToDay;
    timeZones[1] = nightToDay + transitionTime;
    timeZones[2] = dayToNight;
    timeZones[3] = dayToNight + transitionTime;
  }

  void DateTime::reset()
  {
    time = 0.0f;
  }

  void DateTime::setTime(float time)
  {
    this->time = time;
  }

  void DateTime::setScale(float scale)
  {
    this->scale = scale;
  }

  int DateTime::getDay() const
  {
    return day;
  }

  float DateTime::getTime() const
  {
    return time;
  }

  float DateTime::getScale() const
  {
    return scale;
  }

  float DateTime::getDarkness() const
  {
    if (time < timeZones[0])
      return maxDarkness;
    if (time < timeZones[1])
      return 1.0f - clamp((time - timeZones[0]) / transitionTime, 1.0f - maxDarkness, 1.0f - minDarkness);
    if (time < timeZones[2])
      return minDarkness;
    if (time < timeZones[3])
      return clamp((time - timeZones[2]) / transitionTime, minDarkness, maxDarkness);
    return maxDarkness;
  }

  void DateTime::update(float delta)
  {
    time += delta * scale;
    if (time >= 1.0f)
    {
      int days = (int)time;
      day += days;
      time = time - (float)days;
    }
  }

  void DateTime::update(float delta, Environment& environment)
  {
    update(delta);
    environment.darkness = getDarkness();
  }

}
