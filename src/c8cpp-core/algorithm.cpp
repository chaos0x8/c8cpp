#include "c8cpp-core/algorithm.hpp"

namespace C8::Core {
  std::string join(std::span<const std::string_view> collection, std::string_view separator) {
    return Ranges::join(collection, separator);
  }
} // namespace C8::Core
