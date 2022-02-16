#pragma once
#include <flectron/utils/platform.hpp>
#include <flectron/scene/game.hpp>

extern flectron::Game* flectron::createGame();

#ifdef FLECTRON_NO_CONSOLE
  #ifdef FLECTRON_PLATFORM_WINDOWS
    #define NOMINMAX
    #include <windows.h>
    int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
  #else
    #error "Unsupported platform for no console mode!"
  #endif
#else
int main(int argc, char** argv)
#endif
{
  flectron::Game* game = flectron::createGame();
  game->run();
  delete game;

  return 0;
}