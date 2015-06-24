#include "platform.h"

#if defined(Q_OS_WIN)
    #include "platform_win.h"
#elif defined(Q_OS_MAC)
    #include "platform_mac.h"
#elif defined(Q_OS_UNIX)
    #include "platform_x11.h"
#endif

namespace Platform
{
QString getMonitorProfilePath()
{
#if defined(Q_OS_WIN)
    return getMonitorProfilePathWin();
#elif defined(Q_OS_MAC)
    char buffer[512];

    if (get_screen_profile(buffer, 512))
        return QString::fromLocal8Bit(buffer);
    else
        return QString();
#elif defined(Q_OS_UNIX)
    return getMonitorProfilePathX11();
#else
#warning "Monitor profiles not supported on this system."
#endif
    return QString();
}
}
