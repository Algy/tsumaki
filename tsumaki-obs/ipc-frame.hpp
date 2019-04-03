#pragma once
#include <memory>
#include <cstdint>
#include <string>

#include "ipc-error.hpp"
#include "ipc-connection.hpp"

namespace tsumaki::ipc {
    using Message = ::google::protobuf::Message;
    class IPCFrame {
    public:
        enum Type {
            RequestType,
            ResponseType,
            ErrorType
        };
    protected:
        Type frame_type;
        std::shared_ptr<Message> message;
    public:
        IPCFrame(Type frame_type, std::shared_ptr<Message> message) : frame_type(frame_type), message(message) { }
        std::shared_ptr<Message> get_message() const { return message; };
        Type get_frame_type() const { return frame_type; };
        int get_method_number() const;

    public:
        bool is_error() { return frame_type == ErrorType; };
    };

    class IPCFrameChannel {
    private:
        IPCConnection &connection;
    public:
        static void init_frame_channel();
        IPCFrameChannel(IPCConnection &connection) : connection(connection) {};
    public:
        void send(const IPCFrame &frame);
        IPCFrame receive();
    };
}
