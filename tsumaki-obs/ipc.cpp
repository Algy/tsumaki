#include <algorithm>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <csignal>
#include <sys/types.h>
#include <vector>
#include <thread>
#include "ipc.hpp"
#include "protobuf/ErrorResponse.pb.h"

namespace tsumaki {
    IPC::~IPC() {
        base_conn.close();
    }

    IPCConnection IPC::generate_connection() {
        return IPCConnection(host, port);
    }

    void IPC::init_ipc_system() {
        net::init();
#ifdef SIGPIPE
        std::signal(SIGPIPE, SIG_IGN);
#endif
        IPCFrameChannel::init_frame_channel();
    }


    static RPCResult do_request(IPCConnection &conn, std::shared_ptr<Message> message) {
        try {
            conn.ensure_connection();
            IPCFrameChannel channel(conn);

            IPCFrame request_frame(IPCFrame::RequestType, message);
            channel.send(request_frame);
            IPCFrame resp_frame = channel.receive();

            RPCResult result;
            if (resp_frame.is_error()) {
                ErrorResponse& err_message = static_cast<ErrorResponse&>(*resp_frame.get_message());
                result.success = false;
                result.error_code = err_message.code();
                result.error_message = err_message.msg();
            } else {
                result.success = true;
                result.message = resp_frame.get_message();
            }
            return result;
        } catch (IPCConnectionClosedError &err) {
            (void)err;
            conn.close();
            throw;
        }
    }

    RPCResult IPC::request_sync(std::shared_ptr<Message> message) {
        return do_request(base_conn, message);
    }

    void IPC::request_async(
            std::shared_ptr<Message> message,
            std::function<void(RPCResult &result)> callback,
            std::function<void(IPCError &exc)> error) {
        std::shared_ptr<IPCConnection> conn { new IPCConnection(generate_connection()) };
        std::thread([conn, message, callback, error]() {
            try {
                RPCResult rpc_result = do_request(*conn, message);
                callback(rpc_result);
            } catch (IPCError &exc) {
                if (error != nullptr) {
                    error(exc);
                }
                throw exc;
            }
        });
    }
}

