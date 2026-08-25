#pragma once

#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace C8::Core {
  std::string join(std::span<const std::string_view> collection, std::string_view separator);

  namespace Ranges {
    inline std::string join(const auto& collection, std::string_view separator) {
      std::string out;

      size_t i = 0;

      for (auto it = collection.begin(); it != collection.end(); ++it, ++i) {
        if (i > 0) {
          out += separator;
        }

        out += *it;
      }

      return out;
    }

    template <typename Collection, typename F>
    inline auto transform(Collection&& collection, F&& func) {
      std::vector<decltype(func(*collection.begin()))> out;

      for (const auto& item : collection) {
        out.emplace_back(func(item));
      }

      return out;
    }
  } // namespace Ranges
} // namespace C8::Core
