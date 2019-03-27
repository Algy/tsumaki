#include <string>
#include <memory>

namespace tsumaki {
    class PyGlue {
    public:
      int neural_dimension;
      std::string neural_model_branch = "incubator";
      std::string neural_model_type = "deeplabv3mobile";
      std::string neural_model_version = "0.0.1";

      bool use_crf;
      int crf_dimension;

      bool crf_gaussian_required;
      float crf_gaussian_sxy;
      float crf_gaussian_compat;

      float crf_biliteral_sxy;
      float crf_biliteral_srgb;
      float crf_billiteral_compat;
    public:
      PyGlue();
    public:
      void start();
      void stop();
      void wait_stopped();
      void enqueue(uint8_t *data);
    };
};
