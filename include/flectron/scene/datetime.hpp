#pragma once

namespace flectron
{

  struct Environment;

  class DateTime
  {
  private:
    float time;
    float scale;

    float timeZones[4];
    float transitionTime;

    float minDarkness;
    float maxDarkness;

    int day;

  public:
    DateTime(float startTime, float nightToDay, float dayToNight, float transitionTime, float scale, float minDarkness, float maxDarkness);
    DateTime(const DateTime& other) = default;

    void reset();
    void setTime(float time);
    void setScale(float scale);

    int getDay() const;
    float getTime() const;
    float getScale() const;

    float getDarkness() const;

    void update(float delta);
    void update(float delta, Environment& environment);
  };

}
