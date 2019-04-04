#pragma once
#include <memory>
#include "obs-filter.hpp"
#include "platform-def.hpp"
#include "ipc.hpp"


namespace tsumaki {
    class TsumakiFilter : public OBSFilter {
    private:
        std::unique_ptr<ipc::IPC> curr_ipc;
        uint64_t interval;
    public:
        TsumakiFilter();
        ~TsumakiFilter();
        virtual void init();
        virtual void destroy();
        virtual void update_settings(obs_data_t *settings);
        virtual void get_properties(obs_properties_t* props);
        virtual void detach(obs_source_t *parent);
        virtual std::unique_ptr<Frame> frame_update(std::unique_ptr<Frame> frame);
        virtual const char* get_scope_name() const { return "Tsumaki"; };
    };
};

