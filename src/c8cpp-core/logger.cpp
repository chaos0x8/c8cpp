#include "c8cpp-core/logger.hpp"
#include "c8cpp-core/meta.hpp"

#include <filesystem>
#include <format>
#include <utility>

namespace C8::Core {
  Logger::Logger(std::ostream& out) : out_(out) {}

  void Logger::log(LogLevel logLevel, std::string_view s, std::source_location location)
  {
    if (logLevel >= logLevel_) {
      std::print(out_, "[{}] <{}/{}:{}> {}\n", enumToString(logLevel),
        std::filesystem::path(location.file_name()).parent_path().filename(),
        std::filesystem::path(location.file_name()).filename(), location.line(), s);
    }
  }

  void Logger::logLevel(LogLevel logLevel)
  {
    logLevel_ = logLevel;
  }

  LogLevel Logger::logLevel() const
  {
    return logLevel_;
  }

  bool LoggerConfig::isLoggerValid()
  {
    return static_cast<bool>(loggerInstance_);
  }

  void LoggerConfig::setLogger(Logger& logger)
  {
    loggerInstance_ = &logger;
  }

  Logger& LoggerConfig::getLogger()
  {
    return *loggerInstance_;
  }

  Logger* LoggerConfig::loggerInstance_ = nullptr;
} // namespace C8::Core
