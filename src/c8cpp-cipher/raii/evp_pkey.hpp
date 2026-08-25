#pragma once

#include "c8cpp-cipher/raii/raii.hpp"

namespace C8::Cipher {
  template <>
  struct OsslPointerTraits<EVP_PKEY> {
    static EVP_PKEY* create(EVP_PKEY_CTX& evpPkeyCtx);
    static EVP_PKEY* create(FromPublicKey_t, CipherDataRO bytes);
    static EVP_PKEY* create(FromPrivateKey_t, CipherDataRO bytes);
    static EVP_PKEY* create(FromRaw_t, EVP_PKEY* raw);
    static void destroy(EVP_PKEY* ptr);

    virtual ~OsslPointerTraits() = default;

    CipherData get_mem_data() const;
  };
} // namespace C8::Cipher
