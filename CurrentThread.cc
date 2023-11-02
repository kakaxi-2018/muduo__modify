#include "CurrentThread.h"
#include <unistd.h>
#include <syscall.h>

namespace detail
{

pid_t gettid()
{
    return static_cast<pid_t>(::syscall(SYS_gettid));
}

}

namespace CurrentThread
{
    __thread int t_cachedTid = 0;
    __thread char t_tidString[32];
    __thread int t_tidStringLength = 6;
    __thread const char * t_threadName = "unknown";

    void cacheTid()
    {
        if (0 == t_cachedTid)
        {
            // 通过 linux 系统调用获取 当前线程的 tid 值
            t_cachedTid = detail::gettid();
            //t_tidStringLength = snprintf("t_tidString", sizeof(t_tidString), )
        }
    }
}