#pragma once

#include "c8cpp-cipher/raii/raii.hpp"

namespace C8::Cipher {
  template <>
  struct OsslPointerTraits<EVP_PKEY_CTX> {
    static EVP_PKEY_CTX* create();
    static EVP_PKEY_CTX* create(EVP_PKEY& evpPkey);
    static void destroy(EVP_PKEY_CTX* ptr);

    virtual ~OsslPointerTraits() = default;

    void keygen_init();
    void set_rsa_keygen_bits(int bits);
    OsslPointer<EVP_PKEY> keygen();
  };
} // namespace C8::Cipher
