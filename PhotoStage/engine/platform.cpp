#include "platform.h"

#if defined(Q_OS_WIN)
#include "platform_win.h"
#elif defined(Q_OS_MAC)
#include "platform_mac.h"
#elif defined(Q_OS_UNIX)
#include "platform_x11.h"
#endif

namespace Platform {
QString getMonitorProfilePath()
{
#define BUFFER_SIZE 512
  char buffer[BUFFER_SIZE];
#if defined(Q_OS_WIN)
  return getMonitorProfilePathWin();
#elif defined(Q_OS_MAC)
  if (get_screen_profile(buffer, BUFFER_SIZE))
    return QString::fromLocal8Bit(buffer);
#elif defined(Q_OS_UNIX)
  if (get_display_profile_path_x11(buffer, BUFFER_SIZE))
    return QString::fromLocal8Bit(buffer);
#else
#warning "Monitor profiles not supported on this system."
#endif
  return QString();
}
} // namespace Platform
