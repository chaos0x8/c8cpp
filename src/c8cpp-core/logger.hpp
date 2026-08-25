#pragma once

#include <ostream>
#include <source_location>

namespace C8::Core {
  enum class LogLevel { Trace, Debug, Info, Warning, Error, Critical, Silent };

  class Logger {
  public:
    Logger(std::ostream& out);

    void log(LogLevel logLevel, std::string_view s, std::source_location location = std::source_location::current());
    void logLevel(LogLevel);
    LogLevel logLevel() const;

  private:
    std::ostream& out_;
    LogLevel logLevel_ = LogLevel::Info;
  };

  struct LoggerConfig {
    static bool isLoggerValid();
    static void setLogger(Logger&);
    static Logger& getLogger();

  private:
    static Logger* loggerInstance_;
  };
} // namespace C8::Core

#define C8_LOG(LOG_LEVEL, ...)                                                                                         \
  do {                                                                                                                 \
    if (C8::Core::LoggerConfig::isLoggerValid()) {                                                                     \
      C8::Core::LoggerConfig::getLogger().log(LOG_LEVEL, std::format(__VA_ARGS__));                                    \
    }                                                                                                                  \
  } while (false)

#define C8_TRACE(...) C8_LOG(C8::Core::LogLevel::Trace, __VA_ARGS__)
#define C8_DEBUG(...) C8_LOG(C8::Core::LogLevel::Debug, __VA_ARGS__)
#define C8_INFO(...) C8_LOG(C8::Core::LogLevel::Info, __VA_ARGS__)
#define C8_WARNING(...) C8_LOG(C8::Core::LogLevel::Warning, __VA_ARGS__)
#define C8_ERROR(...) C8_LOG(C8::Core::LogLevel::Error, __VA_ARGS__)
#define C8_CRITICAL(...) C8_LOG(C8::Core::LogLevel::Critical, __VA_ARGS__)
