#pragma once

#include <ostream>

namespace MindlessEngine 
{

  struct Vector 
  {
    float x;
    float y;

    Vector();
    Vector(float x, float y);
  };

  Vector operator+(const Vector& a, const Vector& b);
  Vector operator-(const Vector& a, const Vector& b);
  Vector operator-(const Vector& v);
  Vector operator*(const Vector& v, float s);
  Vector operator/(const Vector& v, float s);

  struct Transform;
  Vector transform(const Vector& v, const Transform& transform);

  bool operator==(const Vector& a, const Vector& b);

  std::ostream& operator<<(std::ostream& os, const Vector& v);

};
