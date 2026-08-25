#pragma once

#include <cstddef>
#include <sstream>
#include <string>
#include <string_view>

namespace C8::Db::Utility {
  template <typename... Args>
  inline auto forEach(auto lambda, Args&&... args) {
    return (lambda(std::forward<Args>(args)), ...);
  }

  template <typename... Args>
  inline auto forEachWithIndex(auto lambda, Args&&... args) {
    size_t index = 0;
    return (lambda(std::forward<Args>(args...), index++), ...);
  }

  template <typename... Args>
  inline std::string join(const std::string& separator, Args&&... args) {
    size_t index = 0;

    std::stringstream ss;
    ((ss << (index++ == 0 ? "" : ", ") << args), ...);
    return ss.str();
  }

  template <typename... Args>
  inline auto transform(auto then, auto lambda, Args&&... args) {
    return then(lambda(std::forward<Args>(args))...);
  }
} // namespace C8::Db::Utility
