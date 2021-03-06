#include <flectron/physics/vector.hpp>

namespace flectron 
{

  Vector::Vector() : x(0.0f), y(0.0f) {}
  Vector::Vector(float x, float y) : x(x), y(y) {}

  Vector operator+(const Vector& a, const Vector& b)
  {
    return { a.x + b.x, a.y + b.y };
  }

  Vector operator-(const Vector& a, const Vector& b)
  {
    return { a.x - b.x, a.y - b.y };
  }

  Vector operator-(const Vector& v)
  {
    return { -v.x, -v.y };
  }

  Vector operator*(const Vector& v, float s)
  {
    return { v.x * s, v.y * s };
  }

  Vector operator*(float s, const Vector& v)
  {
    return { v.x * s, v.y * s };
  }

  Vector operator/(const Vector& v, float s)
  {
    return { v.x / s, v.y / s };
  }

  bool operator==(const Vector& a, const Vector& b)
  {
    return a.x == b.x && a.y == b.y;
  }

  std::ostream& operator<<(std::ostream& os, const Vector& v)
  {
    return os << "(" << v.x << ", " << v.y << ")";
  }

}
