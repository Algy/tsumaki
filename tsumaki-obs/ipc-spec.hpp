#pragma once
#include <functional>
#include <vector>
#include <string>
#include <memory>
#include <map>

#include "protobuf/Heartbeat.pb.h"
#include "protobuf/DetectPerson.pb.h"

namespace tsumaki::ipc {
    struct IPCFrameSpec {
        const int method_number;
        const std::string req;
        const std::string resp;
        IPCFrameSpec(int method_number, std::string req, std::string resp) : method_number(method_number), req(req), resp(resp) {};
    };

    std::map<int, const IPCFrameSpec*> _method_to_spec;
    std::map<std::string, const IPCFrameSpec*> _name_to_spec;

    std::vector<IPCFrameSpec> frame_specs = {
        IPCFrameSpec(0, "HeartbeatRequest", "HeartbeatResponse"),
        IPCFrameSpec(1, "DetectPersonRequest", "DetectPersonResponse")
    };

    const IPCFrameSpec* find_spec_by_name(const std::string &name) {
        return _name_to_spec[name];
    }

    const IPCFrameSpec* find_spec_by_method(int method_number) {
        return _method_to_spec[method_number];
    }

    void init_cache() {
        for (auto spec : frame_specs) {
            _method_to_spec[spec.method_number] = &spec;
            _name_to_spec[spec.req] = &spec;
            _name_to_spec[spec.resp] = &spec;
        }
    }

    void init_frame_specs() {
        HeartbeatRequest();
        HeartbeatResponse();

        DetectPersonRequest();
        DetectPersonResponse();

        init_cache();
    }
}
