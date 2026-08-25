#pragma once

#include "c8cpp-cipher/cipher_data.hpp"
#include "c8cpp-cipher/raii.hpp"

#include <openssl/evp.h>

#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace C8::Cipher {
  class AesCipher {
  public:
    static CipherData encrypt(CipherDataRO input, CipherDataRO password);
    static CipherData decrypt(CipherDataRO input, CipherDataRO password);

    static constexpr size_t keySize = 256u / sizeof(uint8_t);
    static constexpr size_t ivSize = 128u / sizeof(uint8_t);
  };
} // namespace C8::Cipher
