#include "c8cpp-core/assert.hpp"

#include "c8cpp-core/error.hpp"

#include <filesystem>

namespace C8::Core {
  std::string AbortFormat(std::string_view message, std::source_location l)
  {
    return std::format("[{}:{}] {}", std::filesystem::path(l.file_name()).filename(), l.line(), message);
  }
} // namespace C8::Core
