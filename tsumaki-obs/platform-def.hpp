#ifndef _WIN32
#include "ipc_platforms/unix.hpp"
namespace tsumaki {
    using AvailableIPC = tsumaki::ipc::UnixIPC;
}
#else
#include "ipc_platforms/win32.hpp"
namespace tsumaki {
    using AvailableIPC = tsumaki::ipc::Win32IPC;
}
#endif
