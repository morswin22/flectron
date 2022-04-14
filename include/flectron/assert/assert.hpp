#pragma once
#include "debug.hpp"
#include <exception>
#include <iostream>
#include <flectron/assert/log.hpp>

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
  #define FLECTRON_ASSERT(condition, message) \
    if (!(condition)) { \
      FLECTRON_LOG_DEBUG("{}:{} Assertion failed: {}", __FILE__, __LINE__, message); \
      FLECTRON_IF_DEBUGGER_PRESENT(FLECTRON_DEBUG_BREAK()); \
      throw flectron::AssertionException(message); \
    }

  #if FLECTRON_ENABLE_DEBUG == 0
    #define __FLECTRON_CATCH_DEBUG_INFO \
      FLECTRON_LOG_WARN("Set FLECTRON_ENABLE_DEBUG to TRUE for more information") 
  #else
    #define __FLECTRON_CATCH_DEBUG_INFO 
  #endif

  #define __FLECTRON_CATCH \
    catch (const flectron::AssertionException& exception) \
    { \
      FLECTRON_LOG_CRITICAL("Assertion failed: {}", exception.what()); \
      __FLECTRON_CATCH_DEBUG_INFO; \
    } \
    catch (const std::exception& exception) \
    { \
      FLECTRON_LOG_ERROR("Unhandled exception: {}", exception.what()); \
    } \
    catch (...) \
    { \
      FLECTRON_LOG_ERROR("Unhandled exception"); \
    }
#else
  #define FLECTRON_ASSERT(condition, message)
  #define __FLECTRON_CATCH
#endif