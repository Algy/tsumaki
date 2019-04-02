#include <exception>

namespace tsumaki::ipc {
    class IPCError : public std::runtime_error {
    public:
        std::string what_message;
        IPCError(std::string str) : what_message(str) {};
        const char* what() override
        {
            return what_message.c_str();
        }
    };

    class IPCFormatError : public IPCError {};
    class IPCConnectionError : public IPCError {};
    class IPCConnectionClosedError : public IPCConnectionError {};

}
