#include <exception>
#include <functional>
#include <memory>
#include <string>
#include <wstring>
#include "protobuf/DetectPerson.pb.h"
#include "deps/xsocket.hpp"

namespace tsumaki::ipc {
    class IPCType {
        bool use_tcp;
        std::string unix_socket;
        std::string host;
        int port;
    };

    class IPCFailedError : std::exception {
        using std::exception::exception;
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
        IPCType ipc_type;
        net::socket socket;j
    public:
        static init_ipc_system();
        IPC(const std::string host = "localhost", int port = 1125) : {
            ipc_type.use_tcp = true;
            ipc_type.host = host;
            ipc_type.port = port;
        };

        IPC(const std::string unix_socket = "/tmp/tsumaki.sock") : {
            ipc_type.use_tcp = false;
            ipc_type.unix_socket = unix_socket;
        };

        bool check_process();
        bool spawn_process();
        void terminate_process();

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
