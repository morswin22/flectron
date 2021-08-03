#include <ostream>

namespace MindlessEngine 
{

  struct Vector 
  {
    float x;
    float y;

    Vector(float x, float y);

    friend Vector operator+(const Vector& a, const Vector& b);
    friend std::ostream& operator<<(std::ostream& os, const Vector& v);
  };

  Vector operator+(const Vector& a, const Vector& b);
  std::ostream& operator<<(std::ostream& os, const Vector& v);

};
