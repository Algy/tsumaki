#pragma once
#include <string>

namespace tsumaki::ipc {
    struct IPCFrameSpec {
        const int method_number;
        const std::string req;
        const std::string resp;
        IPCFrameSpec(int method_number, std::string req, std::string resp) : method_number(method_number), req(req), resp(resp) {};
    };

    void init_frame_specs();
    const IPCFrameSpec* find_spec_by_method(int method_number);
    const IPCFrameSpec* find_spec_by_name(const std::string &name);
}
