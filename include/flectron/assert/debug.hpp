#include "psnip_trap.hpp"

#ifndef FLECTRON_ENABLE_DEBUG
  #ifndef NDEBUG
    #define FLECTRON_ENABLE_DEBUG 1
  #else
    #define FLECTRON_ENABLE_DEBUG 0
  #endif
#endif

#if FLECTRON_ENABLE_DEBUG
  #define FLECTRON_DEBUG_BREAK() psnip_trap()
#else
  #define FLECTRON_DEBUG_BREAK() 
#endif