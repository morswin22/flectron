#pragma once

#ifdef _WIN32
  #ifdef _WIN64
    #define FLECTRON_PLATFORM_WINDOWS
  #else
    #define FLECTRON_PLATFORM_WINDOWS
    #warning "Not tested on 32-bit Windows"
  #endif
#elif defined(__APPLE__) || defined(__MACH__)
  #include <TargetConditionals.h>
  #if TARGET_IPHONE_SIMULATOR == 1
    #error "IOS simulator is not supported!"
  #elif TARGET_OS_IPHONE == 1
    #define FLECTRON_PLATFORM_IOS
    #error "IOS is not supported!"
  #elif TARGET_OS_MAC == 1
    #define FLECTRON_PLATFORM_MACOS
    #warning "Not tested on MacOS"
  #else
    #error "Unknown Apple platform!"
  #endif
#elif defined(__ANDROID__)
  #define FLECTRON_PLATFORM_ANDROID
  #error "Android is not supported!"
#elif defined(__linux__)
  #define FLECTRON_PLATFORM_LINUX
  #warning "Not tested on Linux"
#else
  #error "Unknown platform!"
#endif