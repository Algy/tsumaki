#pragma once
#include <functional>
#include <memory>
#include <string>
#include "protobuf/DetectPerson.pb.h"
#include "ipc-connection.hpp"
#include "ipc-frame.hpp"

namespace tsumaki::ipc {
    class RPCResult {
    public:
        bool success;
        std::shared_ptr<Message> message;
        std::string error_message;
        int error_code;
    };

    class IPC {
    private:
        std::string host;
        int port;
        IPCConnection base_conn;
    public:
        static void init_ipc_system();
        IPC(std::string host = "127.0.0.1", int port = 1125) : host(host), port(port), base_conn(host, port) {};
        virtual ~IPC();
        bool check_process();
        bool spawn_process();
        void terminate_process();
        RPCResult request_sync(std::shared_ptr<Message> message);
        void request_async(
            std::shared_ptr<Message> message,
            std::function<void(RPCResult &result)> callback,
            std::function<void(IPCError &exc)> error = nullptr
        );
    protected:
        IPCConnection generate_connection();
    };
};
