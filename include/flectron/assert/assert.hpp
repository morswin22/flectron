#pragma once
#include "debug.hpp"
#include <exception>
#include <iostream>

#ifndef FLECTRON_ENABLE_ASSERT
  #define FLECTRON_ENABLE_ASSERT 1
#endif

namespace flectron
{

  class AssertionException : public std::runtime_error
  {
  public:
    AssertionException(const std::string& message) : std::runtime_error(message) {}
  };

}

#if FLECTRON_ENABLE_ASSERT
  #if FLECTRON_ENABLE_DEBUG
    #define FLECTRON_ASSERT(condition, message) \
      if (!(condition)) { \
        std::cerr << "Flectron assertion failed in " << __FILE__ << ":" << __LINE__ << " saying: " << message << std::endl; \
        FLECTRON_DEBUG_BREAK(); \
        throw flectron::AssertionException(message); \
      }
  #else
    #define FLECTRON_ASSERT(condition, message) \
      if (!(condition)) { \
        throw flectron::AssertionException(message); \
      }
  #endif
#else
  #define FLECTRON_ASSERT(condition, message)
#endif