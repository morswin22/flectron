#pragma once

namespace flectron
{

  struct Color
  {
    float r, g, b, a;

    Color(float r, float g, float b, float a);
    Color(float r, float g, float b);
    Color(float grey);
  };

  namespace Colors
  {
    Color white();
    Color black();
    Color red();
    Color green();
    Color blue();
    Color yellow();
    Color cyan();
    Color magenta();
    Color orange();
    Color purple();
    Color gray();
    Color darkGray();
    Color lightGray();
    Color darkRed();
    Color darkGreen();
    Color darkBlue();
    Color darkYellow();
    Color darkCyan();
    Color darkMagenta();
    Color darkOrange();
    Color darkPurple();
    Color lightRed();
    Color lightGreen();
    Color lightBlue();
    Color lightYellow();
    Color lightCyan();
    Color lightMagenta();
    Color lightOrange();
    Color lightPurple();
    Color random();
  }

}
