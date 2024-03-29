#include <flectron/renderer/color.hpp>

#include <flectron/utils/random.hpp>

namespace flectron
{

  Color::Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}
  Color::Color(float r, float g, float b) : r(r), g(g), b(b), a(1.0f) {}
  Color::Color(float grey) : r(grey), g(grey), b(grey), a(1.0f) {}

  namespace Colors
  {
    Color white() { return Color(1.0f, 1.0f, 1.0f, 1.0f); }
    Color black() { return Color(0.0f, 0.0f, 0.0f, 1.0f); }
    Color red() { return Color(1.0f, 0.0f, 0.0f, 1.0f); }
    Color green() { return Color(0.0f, 1.0f, 0.0f, 1.0f); }
    Color blue() { return Color(0.0f, 0.0f, 1.0f, 1.0f); }
    Color yellow() { return Color(1.0f, 1.0f, 0.0f, 1.0f); }
    Color cyan() { return Color(0.0f, 1.0f, 1.0f, 1.0f); }
    Color magenta() { return Color(1.0f, 0.0f, 1.0f, 1.0f); }
    Color orange() { return Color(1.0f, 0.5f, 0.0f, 1.0f); }
    Color purple() { return Color(0.5f, 0.0f, 0.5f, 1.0f); }
    Color gray() { return Color(0.5f, 0.5f, 0.5f, 1.0f); }
    Color darkGray() { return Color(0.25f, 0.25f, 0.25f, 1.0f); }
    Color lightGray() { return Color(0.75f, 0.75f, 0.75f, 1.0f); }
    Color darkRed() { return Color(0.5f, 0.0f, 0.0f, 1.0f); }
    Color darkGreen() { return Color(0.0f, 0.5f, 0.0f, 1.0f); }
    Color darkBlue() { return Color(0.0f, 0.0f, 0.5f, 1.0f); }
    Color darkYellow() { return Color(0.5f, 0.5f, 0.0f, 1.0f); }
    Color darkCyan() { return Color(0.0f, 0.5f, 0.5f, 1.0f); }
    Color darkMagenta() { return Color(0.5f, 0.0f, 0.5f, 1.0f); }
    Color darkOrange() { return Color(0.5f, 0.25f, 0.0f, 1.0f); }
    Color darkPurple() { return Color(0.25f, 0.0f, 0.25f, 1.0f); }
    Color lightRed() { return Color(1.0f, 0.5f, 0.5f, 1.0f); }
    Color lightGreen() { return Color(0.5f, 1.0f, 0.5f, 1.0f); }
    Color lightBlue() { return Color(0.5f, 0.5f, 1.0f, 1.0f); }
    Color lightYellow() { return Color(1.0f, 1.0f, 0.5f, 1.0f); }
    Color lightCyan() { return Color(0.5f, 1.0f, 1.0f, 1.0f); }
    Color lightMagenta() { return Color(1.0f, 0.5f, 1.0f, 1.0f); }
    Color lightOrange() { return Color(1.0f, 0.75f, 0.5f, 1.0f); }
    Color lightPurple() { return Color(0.75f, 0.5f, 0.75f, 1.0f); }
    Color random() 
    { 
      std::uniform_real_distribution<float> dist(0.0f, 1.0f);
      return Color(dist(randomEngine), dist(randomEngine), dist(randomEngine), 1.0f); 
    }
  }

}
