#pragma once
#include <exception>

namespace tsumaki::ipc {
    class IPCError : public std::exception {
    public:
        std::string what_message;
        IPCError(std::string str) : what_message(str) {};
        const char* what() noexcept
        {
            return what_message.c_str();
        }
    };

    class IPCFormatError : public IPCError {
        using IPCError::IPCError;
    };
    class IPCConnectionError : public IPCError {
        using IPCError::IPCError;
    };
    class IPCConnectionClosedError : public IPCConnectionError {
        using IPCConnectionError::IPCConnectionError;
    };

}
