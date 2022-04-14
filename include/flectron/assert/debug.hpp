#include "psnip_trap.hpp"

#ifndef FLECTRON_ENABLE_DEBUG
  #ifndef NDEBUG
    #define FLECTRON_ENABLE_DEBUG 1
  #else
    #define FLECTRON_ENABLE_DEBUG 0
  #endif
#endif

namespace flectron
{

  bool isDebuggerPresent();

}

#if FLECTRON_ENABLE_DEBUG
  #define FLECTRON_DEBUG_BREAK() psnip_trap()
  #define FLECTRON_IS_DEBUGGER_PRESENT() flectron::isDebuggerPresent()
  #define FLECTRON_IF_DEBUGGER_PRESENT(statement) if (FLECTRON_IS_DEBUGGER_PRESENT()) statement
#else
  #define FLECTRON_DEBUG_BREAK() 
  #define FLECTRON_IS_DEBUGGER_PRESENT() false
  #define FLECTRON_IF_DEBUGGER_PRESENT(statement)
#endif