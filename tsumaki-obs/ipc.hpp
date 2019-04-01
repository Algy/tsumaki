#include <exception>
#include <functional>
#include <memory>
#include <string>
#include <wstring>
#include "protobuf/DetectPerson.pb.h"

namespace tsumaki::ipc {
    class IPCFailedError : std::exception {
        std::exception::exception;
    };

    class RPCResult {
    public:
        bool success;
        int error_code;
        std::string error_message;
        std::shared_ptr<::google::protobuf::Message> message;
    };

    class IPC {
    private:
        bool use_tcp;
        std::string unix_socket;
        std::string host;
        int port;
    public:
        IPC(const std::string host = "localhost", int port = 1125) : host(host), port(port), use_tcp(true) {};
        IPC(const std::string unix_socket = "/tmp/tsumaki.sock") : unix_socket(unix_socket), use_tcp(false) {};
        bool check_process();
        std::wstring get_pid_file_name();
        bool spawn_process();
        void terminate_process();
        void start_async_thread();

        RPCResult request_sync(
            const ::google::protobuf::Message &message,
            int timeout = 0
        );
        void request_async(
            const ::google::protobuf::Message &message,
            std::function<void(RPCResult &result)> callback,
            int timeout = 0
        );
        void heartbeat(int timeout);
    };
};
