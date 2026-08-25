#pragma once

#include "c8cpp-cipher/cipher_data.hpp"

#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace C8::Cipher {
  template <typename T>
  concept TrivialEncode = std::is_standard_layout_v<T> && std::is_arithmetic_v<T>;

  class HexEncoder {
  public:
    static std::string encode(const CipherDataRO&);
    static CipherData decode(std::string_view);

    class IO {
    public:
      IO() = default;
      IO(std::string);

      IO& operator<<(const CipherDataRO&);
      IO& operator>>(CipherDataRW&);
      IO& operator>>(CipherData&);

      std::string_view hex() const {
        return hex_;
      }

      std::string takeHex() {
        return std::exchange(hex_, std::string{});
      }

      size_t size() const {
        return hex_.size();
      }

    private:
      std::string hex_;
    };
  };
} // namespace C8::Cipher
