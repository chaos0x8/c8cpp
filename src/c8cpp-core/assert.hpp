#pragma once

#include "c8cpp-core/error.hpp"

#include <format>
#include <source_location>
#include <string_view>

namespace C8::Core {
  std::string AbortFormat(std::string_view message, std::source_location = std::source_location::current());

  template <typename ErrorType = Error::AssertError>
  [[noreturn]] inline void Abort(std::string_view message, std::source_location l = std::source_location::current())
  {
    throw ErrorType(AbortFormat(message, l));
  }
} // namespace C8::Core

#define C8_ABORTT(type, message, ...)                                                                                  \
  do {                                                                                                                 \
    C8::Core::Abort<type>(std::format(message __VA_OPT__(, __VA_ARGS__)));                                             \
  } while (false)

#define C8_ABORT(message, ...) C8_ABORTT(, message, __VA_ARGS__)

#define C8_ASSERTT(type, condition, message, ...)                                                                      \
  do {                                                                                                                 \
    if (!(condition))                                                                                                  \
      [[unlikely]]                                                                                                     \
      {                                                                                                                \
        C8::Core::Abort<type>(std::format(message __VA_OPT__(, __VA_ARGS__)));                                         \
      }                                                                                                                \
  } while (false)

#define C8_ASSERT(condition, message, ...) C8_ASSERTT(, condition, message, __VA_ARGS__)
