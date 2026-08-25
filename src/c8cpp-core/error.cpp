#include "c8cpp-core/error.hpp"

#include <format>

// FILE IS GENERATED

namespace C8::Core::Error {
  Error::Error(std::string_view what) : what_(what) {}

  AssertError::AssertError(std::string_view detail) : Error(std::format("C8::Core::AssertError: {}", detail)) {}

  StandardError::StandardError(std::string_view detail) : Error(std::format("C8::Core::StandardError: {}", detail)) {}

  ArgumentError::ArgumentError(std::string_view detail) : Error(std::format("C8::Core::ArgumentError: {}", detail)) {}

} // namespace C8::Core::Error
