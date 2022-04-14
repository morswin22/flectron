#include <flectron/assert/debug.hpp>
#include <flectron/utils/platform.hpp>

namespace flectron
{

#ifdef FLECTRON_PLATFORM_WINDOWS
  #include <windows.h>
  bool isDebuggerPresent()
  {
    return IsDebuggerPresent();
  }
#elif defined(FLECTRON_PLATFORM_LINUX)
  // From https://stackoverflow.com/a/24969863
  #include <sys/stat.h>
  #include <string.h>
  #include <fcntl.h>
  #include <unistd.h>
  #include <ctype.h>
  bool isDebuggerPresent()
  {
    char buf[4096];

    const int status_fd = ::open("/proc/self/status", O_RDONLY);
    if (status_fd == -1)
      return false;

    const ssize_t num_read = ::read(status_fd, buf, sizeof(buf) - 1);
    ::close(status_fd);

    if (num_read <= 0)
      return false;

    buf[num_read] = '\0';
    constexpr char tracerPidString[] = "TracerPid:";
    const auto tracer_pid_ptr = ::strstr(buf, tracerPidString);
    if (!tracer_pid_ptr)
      return false;

    for (const char* characterPtr = tracer_pid_ptr + sizeof(tracerPidString) - 1; characterPtr <= buf + num_read; ++characterPtr)
    {
      if (::isspace(*characterPtr))
        continue;
      else
        return ::isdigit(*characterPtr) != 0 && *characterPtr != '0';
    }

    return false;
  }
#else
  #warning "flectron::isDebuggerPresent() not implemented for this platform and will always return true"
  bool isDebuggerPresent()
  {
    return true;
  }
#endif

}