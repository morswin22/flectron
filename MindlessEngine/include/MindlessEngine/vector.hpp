#pragma once

#include <ostream>
#include <MindlessEngine/math.hpp>

namespace MindlessEngine 
{

  struct Vector 
  {
    float x;
    float y;

    Vector();
    Vector(float x, float y);

    friend Vector operator+(const Vector& a, const Vector& b);
    friend Vector operator-(const Vector& a, const Vector& b);
    friend Vector operator-(const Vector& v);
    friend Vector operator*(const Vector& v, float s);
    friend Vector operator/(const Vector& v, float s);

    friend bool operator==(const Vector& a, const Vector& b);

    friend float length(const Vector& v);
    friend float lengthSquared(const Vector& v);
    friend float distance(const Vector& a, const Vector& b);
    friend float distanceSquared(const Vector& a, const Vector& b);
    friend Vector normalize(const Vector& v);
    friend float dot(const Vector& a, const Vector& b);
    friend float cross(const Vector& a, const Vector& b);

    friend std::ostream& operator<<(std::ostream& os, const Vector& v);
  };

  Vector operator+(const Vector& a, const Vector& b);
  Vector operator-(const Vector& a, const Vector& b);
  Vector operator-(const Vector& v);
  Vector operator*(const Vector& v, float s);
  Vector operator/(const Vector& v, float s);

  bool operator==(const Vector& a, const Vector& b);

  std::ostream& operator<<(std::ostream& os, const Vector& v);

};
