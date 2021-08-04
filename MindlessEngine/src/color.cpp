#include <MindlessEngine/color.hpp>

#include <MindlessEngine/random.hpp>

namespace MindlessEngine
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
    Color random() { return Color(randomFloat(0.0f, 1.0f), randomFloat(0.0f, 1.0f), randomFloat(0.0f, 1.0f), 1.0f); }
  };

};