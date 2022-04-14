#pragma once

#include <spdlog/spdlog.h>
#include <flectron/utils/memory.hpp>
#include <flectron/assert/debug.hpp>

#ifndef FLECTRON_ENABLE_LOG
  #define FLECTRON_ENABLE_LOG 1
#endif

#define FLECTRON_LOG_SERVERITY_TRACE 0
#define FLECTRON_LOG_SERVERITY_DEBUG 1
#define FLECTRON_LOG_SERVERITY_INFO 2
#define FLECTRON_LOG_SERVERITY_WARN 3
#define FLECTRON_LOG_SERVERITY_ERROR 4
#define FLECTRON_LOG_SERVERITY_CRITICAL 5

#ifndef FLECTRON_LOG_SEVERITY
  #if FLECTRON_ENABLE_DEBUG
    #define FLECTRON_LOG_SEVERITY FLECTRON_LOG_SERVERITY_TRACE
  #else
    #define FLECTRON_LOG_SEVERITY FLECTRON_LOG_SERVERITY_INFO
  #endif
#endif

namespace flectron
{

  class Log
  {
  public:
    static void init();

    static Ref<spdlog::logger>& getLogger() { return logger; }

  private:
    static Ref<spdlog::logger> logger;
  };

}

#if FLECTRON_ENABLE_LOG
  #define FLECTRON_LOG(level, ...) flectron::Log::getLogger()->level(__VA_ARGS__)
#else
  #define FLECTRON_LOG(level, ...)
#endif

#if FLECTRON_LOG_SEVERITY <= FLECTRON_LOG_SERVERITY_TRACE
  #define FLECTRON_LOG_TRACE(...) FLECTRON_LOG(trace, __VA_ARGS__)
#else
  #define FLECTRON_LOG_TRACE(...)
#endif

#if FLECTRON_LOG_SEVERITY <= FLECTRON_LOG_SERVERITY_DEBUG
  #define FLECTRON_LOG_DEBUG(...) FLECTRON_LOG(debug, __VA_ARGS__)
#else
  #define FLECTRON_LOG_DEBUG(...)
#endif

#if FLECTRON_LOG_SEVERITY <= FLECTRON_LOG_SERVERITY_INFO
  #define FLECTRON_LOG_INFO(...) FLECTRON_LOG(info, __VA_ARGS__)
#else
  #define FLECTRON_LOG_INFO(...)
#endif

#if FLECTRON_LOG_SEVERITY <= FLECTRON_LOG_SERVERITY_WARN
  #define FLECTRON_LOG_WARN(...) FLECTRON_LOG(warn, __VA_ARGS__)
#else
  #define FLECTRON_LOG_WARN(...)
#endif

#if FLECTRON_LOG_SEVERITY <= FLECTRON_LOG_SERVERITY_ERROR
  #define FLECTRON_LOG_ERROR(...) FLECTRON_LOG(error, __VA_ARGS__)
#else
  #define FLECTRON_LOG_ERROR(...)
#endif

#if FLECTRON_LOG_SEVERITY <= FLECTRON_LOG_SERVERITY_CRITICAL
  #define FLECTRON_LOG_CRITICAL(...) FLECTRON_LOG(critical, __VA_ARGS__)
#else
  #define FLECTRON_LOG_CRITICAL(...)
#endif