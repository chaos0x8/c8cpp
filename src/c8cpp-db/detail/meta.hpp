#pragma once

#include "c8cpp-core/meta.hpp"

#include <meta>
#include <sstream>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

namespace C8::Db::Detail {
  template <typename T>
  inline consteval std::string_view dbTypeName()
  {
    using U = std::decay_t<T>;

    if constexpr (std::is_enum_v<U>) {
      return dbTypeName<std::underlying_type_t<U>>();
    } else if constexpr (std::is_same_v<U, std::string>) {
      return "string";
    } else if constexpr (std::is_same_v<U, int32_t>) {
      return "integer";
    } else if constexpr (std::is_same_v<U, int64_t>) {
      return "integer";
    } else if constexpr (std::is_same_v<U, double>) {
      return "double";
    } else {
      static_assert(
        false, std::format("Type '{}' not supported.", std::meta::display_string_of(std::meta::dealias(^^U))));
    }
  }
} // namespace C8::Db::Detail
