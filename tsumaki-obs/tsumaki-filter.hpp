#pragma once
#include <memory>
#include "obs-filter.hpp"
#include "tsumaki-api-thread.hpp"


namespace tsumaki {
    class TsumakiFilter : public OBSFilter {
    private:
        uint64_t interval;
        std::shared_ptr<ApiThread> api_thread;
    public:
        TsumakiFilter();
        ~TsumakiFilter();
        virtual void init();
        virtual void destroy();
        virtual void run_once();
        virtual void update_settings(obs_data_t *settings);
        virtual void get_properties(obs_properties_t* props);
        virtual void detach(obs_source_t *parent);
        virtual std::unique_ptr<Frame> frame_update(std::unique_ptr<Frame> frame);
        virtual const char* get_scope_name() const { return "Tsumaki"; };
    };
};

