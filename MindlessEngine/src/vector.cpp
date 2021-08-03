#include <MindlessEngine/vector.hpp>

namespace MindlessEngine 
{

  Vector::Vector(float x, float y) : x(x), y(y) {}

  Vector operator+(const Vector& a, const Vector& b)
  {
    return { a.x + b.x, a.y + b.y };
  }

  std::ostream& operator<<(std::ostream& os, const Vector& v)
  {
    return os << "(" << v.x << ", " << v.y << ")";
  }

};