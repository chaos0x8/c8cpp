#pragma once

#include "c8cpp-core/assert.hpp"

#include <meta>
#include <sstream>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

namespace C8::Core {
  template <typename T>
  inline consteval std::string_view enumValueToString(T Value)
  {
    template for (constexpr auto info : std::define_static_array(std::meta::enumerators_of(std::meta::dealias(^^T))))
    {
      if (Value == [:info:]) {
        return std::define_static_string(
          std::format("{}::{}", std::meta::identifier_of(std::meta::dealias(^^T)), std::meta::identifier_of(info)));
      }
    }

    std::unreachable();
  }

  template <typename T>
  inline constexpr T stringToEnum(std::string_view v)
  {
    template for (constexpr auto info : std::define_static_array(std::meta::enumerators_of(^^T)))
    {
      if (v == enumValueToString<T>([:info:])) {
        return [:info:];
      }
    }

    C8_ABORT("Cannot convert '{}' to {}", v, std::meta::identifier_of(std::meta::dealias(^^T)));
  }

  template <typename T>
  inline constexpr std::string_view enumToString(T v)
  {
    template for (constexpr auto info : std::define_static_array(std::meta::enumerators_of(^^T)))
    {
      if (v == [:info:]) {
        return enumValueToString<T>([:info:]);
      }
    }

    C8_ABORT("Cannot convert '{}' to {}", std::to_underlying(v), std::meta::identifier_of(std::meta::dealias(^^T)));
  }
} // namespace C8::Core
