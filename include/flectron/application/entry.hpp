#pragma once
#include <flectron/utils/platform.hpp>
#include <flectron/application/application.hpp>
#include <flectron/assert/assert.hpp>

extern flectron::Scope<flectron::Application> flectron::createApplication(const flectron::ApplicationArguments& arguments);

#if defined(FLECTRON_PLATFORM_WINDOWS) && defined(FLECTRON_NO_CONSOLE)
#define NOMINMAX
#include <windows.h>
int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
  // Solution from https://stackoverflow.com/a/57941046
  int argc;
  LPWSTR* szArglist = CommandLineToArgvW(GetCommandLineW(), &argc);
  char** argv = new char*[argc];
  for (int i=0; i<argc; i++) {
    size_t lgth = wcslen(szArglist[i]);
    argv[i] = new char[lgth+1];
    for (size_t j=0; j<=lgth; j++)
      argv[i][j] = char(szArglist[i][j]);
  }
  LocalFree(szArglist);
  flectron::ApplicationArguments arguments{ argc, argv };
#else
int main(int argc, char** argv)
{
  flectron::ApplicationArguments arguments{ argc, argv };
#endif

  {
    auto application = flectron::createApplication(arguments);

    try
    {
      application->run();
    }
    catch (const flectron::AssertionException& exception)
    {
      std::cerr << "Flectron assertion failed saying: " << exception.what() << "\nSet FLECTRON_ENABLE_DEBUG to TRUE for more information" << std::endl;
    }
    catch (const std::exception& exception)
    {
      std::cerr << exception.what() << std::endl;
    }
  }

  return 0;
}