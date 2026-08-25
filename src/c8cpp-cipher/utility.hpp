#pragma once

#include "c8cpp-cipher/cipher_data.hpp"

#include <cstdint>
#include <filesystem>
#include <span>
#include <string>
#include <type_traits>
#include <vector>

namespace C8::Cipher::Utility {
  std::string toString(std::span<const std::byte>);
  std::string_view asString(std::span<const std::byte>);
  CipherData randomBytes(size_t size);
  std::vector<std::byte> readFile(const std::filesystem::path&);

  template <typename T>
  inline auto* asU8(T* in) {
    using R = std::conditional_t<std::is_const_v<T>, const uint8_t, uint8_t>*;
    static_assert(sizeof(T) == sizeof(uint8_t), "Invalid parameter");
    return reinterpret_cast<R>(in);
  }

  template <typename T>
  inline auto* asChar(T* in) {
    using R = std::conditional_t<std::is_const_v<T>, const char, char>*;
    static_assert(sizeof(T) == sizeof(char), "Invalid parameter");
    return reinterpret_cast<R>(in);
  }

  template <typename T>
  inline constexpr std::span<const std::byte> asBytes(T input) {
    if constexpr (std::is_same_v<T, std::span<const std::byte>>) {
      return input;
    } else if constexpr (std::is_same_v<T, std::string_view>) {
      return std::as_bytes(std::span(input));
    } else if constexpr (std::is_same_v<T, const char*>) {
      return std::as_bytes(std::span(std::string_view(input)));
    } else {
      return std::as_bytes(input);
    }
  }
} // namespace C8::Cipher::Utility
