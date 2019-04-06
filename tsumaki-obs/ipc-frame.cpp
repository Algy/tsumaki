#include <google/protobuf/message.h>
#include <unordered_map>
#include <iostream>
#include "ipc-spec.hpp"
#include "ipc-frame.hpp"

namespace tsumaki {
    const std::string header_sig = "@(^o^)=@";
    const std::string trailer_sig = "@=(^o^)@";
    const int current_version = 1;
   
    static char frametype2char(IPCFrame::Type type) {
        switch (type) {
            case IPCFrame::RequestType:
                return 'Q';
            case IPCFrame::ResponseType:
                return 'P';
            case IPCFrame::ErrorType:
                return 'E';
            default:
                throw "Not reachable";
        }
    }

    static std::string s_u16_le(uint16_t num) {
        char buf[2] { (char)(num & 0xFF), (char)((num >> 8) & 0xFF) };
        return std::string(buf, 2);
    }

    static int16_t p_u16_le(const std::string &s, int pos) {
        return ((uint8_t)s[pos + 1] << 8) + (uint8_t)s[pos];
    }

    static std::string s_u32_le(uint32_t num) {
        char buf[4] = {
            (char)(num & 0xFF),
            (char)((num >> 8) & 0xFF),
            (char)((num >> 16) & 0xFF),
            (char)((num >> 24) & 0xFF),
        };
        return std::string(buf, 4);
    }

    static uint32_t p_u32_le(const std::string &s, int pos) {
        return ((uint8_t)s[pos + 3] << 24) + ((uint8_t)s[pos + 2] << 16) + ((uint8_t)s[pos + 1] << 8) + (uint8_t)s[pos];
    }

    static IPCFrame::Type framechar2type(char c) {
        switch (c) {
            case 'Q':
                return IPCFrame::RequestType;
            case 'P':
                return IPCFrame::ResponseType;
            case 'E':
                return IPCFrame::ErrorType;
            default:
                {
                    std::string err = "Unexpected frame type ";
                    err += c;
                    throw IPCFormatError(err);
                }
        }
    }
    

    void IPCFrameChannel::send(const IPCFrame &frame) {
        std::string body_buf;
        frame.get_message()->SerializeToString(&body_buf);

        std::string header_buf = header_sig;
        header_buf += s_u16_le(current_version);
        header_buf += frametype2char(frame.get_frame_type());

        int method_number = frame.get_method_number();
        header_buf += s_u16_le(method_number);
        header_buf += s_u32_le(body_buf.size());

        connection.write_all(header_buf);
        connection.write_all(body_buf);
        connection.write_all(trailer_sig);
    }

    IPCFrame IPCFrameChannel::receive() {
        std::string header_buf = connection.read_all(17);
        if (header_buf.substr(0, 8) != header_sig) { throw IPCFormatError("Invalid header signature"); }
        uint16_t version = p_u16_le(header_buf, 8);
        char frame_type_c = (char)header_buf[10];
        uint16_t method_number = p_u16_le(header_buf, 11);
        uint32_t body_length = p_u32_le(header_buf, 13);

        if (version != current_version) {
            throw IPCFormatError("Version mismatch");
        }
        IPCFrame::Type frame_type = framechar2type(frame_type_c);

        std::string body_buf = connection.read_all(body_length);
        std::string trailer_buf = connection.read_all(8);

        if (trailer_buf != trailer_sig) {
            throw IPCFormatError("Invalid trailer signature");
        }

        auto spec = find_spec_by_method(method_number);
        if (!spec) {
            throw IPCFormatError("Unknown method number " + std::to_string(method_number));
        }

        std::string message_name;
        if (frame_type == IPCFrame::RequestType) {
            message_name = spec->req;
        } else if (frame_type == IPCFrame::ResponseType) {
            message_name = spec->resp;
        } else {
            throw IPCFormatError("Not appropriate frame type");
        }

        auto desc = google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(message_name);
        if (desc == nullptr) {
            throw IPCFormatError("Can't find the protobuf description of " + message_name);
        }
        std::shared_ptr<Message> message { google::protobuf::MessageFactory::generated_factory()->GetPrototype(desc)->New() };
        if (message == nullptr) {
            throw IPCFormatError("Null message of " + message_name);
        }
        if (!message->ParseFromString(body_buf)) {
            throw IPCFormatError("Failed to parse body");
        }
        return IPCFrame(frame_type, message);
    }

    void IPCFrameChannel::init_frame_channel() {
        init_frame_specs();
    }

    int IPCFrame::get_method_number() const {
        auto spec = find_spec_by_name(message->GetTypeName());
        if (spec == nullptr) return -1;
        return spec->method_number;
    };
}
