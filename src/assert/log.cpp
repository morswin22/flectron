#include <flectron/assert/log.hpp>

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace flectron
{

  Ref<spdlog::logger> Log::logger;

  void Log::init()
  {
    std::vector<spdlog::sink_ptr> sinks;
    sinks.emplace_back(createRef<spdlog::sinks::stdout_color_sink_mt>());
    sinks.emplace_back(createRef<spdlog::sinks::basic_file_sink_mt>("flectron.log", true));

    sinks[0]->set_pattern("%^[%T] %n: %v%$");
    sinks[1]->set_pattern("[%T] [%l] %n: %v");  

    logger = createRef<spdlog::logger>("flectron", sinks.begin(), sinks.end());
    spdlog::register_logger(logger);
    logger->set_level(static_cast<spdlog::level::level_enum>(FLECTRON_LOG_SEVERITY));
    logger->flush_on(spdlog::level::trace);

    logger->trace("Logging initialized");
  }

}