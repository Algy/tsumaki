#include <map>
#include <vector>
#include <memory>

#include "ipc-spec.hpp"
#include "protobuf/Heartbeat.pb.h"
#include "protobuf/DetectPerson.pb.h"


namespace tsumaki::ipc {
    static std::map<int, const IPCFrameSpec*> _method_to_spec;
    static std::map<std::string, const IPCFrameSpec*> _name_to_spec;
    static std::vector<IPCFrameSpec> frame_specs = {
        IPCFrameSpec(0, "HeartbeatRequest", "HeartbeatResponse"),
        IPCFrameSpec(1, "DetectPersonRequest", "DetectPersonResponse")
    };


    static void init_cache() {
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

    const IPCFrameSpec* find_spec_by_method(int method_number) {
        return _method_to_spec[method_number];
    }

    const IPCFrameSpec* find_spec_by_name(const std::string &name) {
        return _name_to_spec[name];
    }
};
