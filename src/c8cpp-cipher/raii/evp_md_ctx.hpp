#pragma once

#include "c8cpp-cipher/raii/raii.hpp"

namespace C8::Cipher {
  template <>
  struct OsslPointerTraits<EVP_MD_CTX> {
    static EVP_MD_CTX* create();
    static void destroy(EVP_MD_CTX* ptr);

    virtual ~OsslPointerTraits() = default;
  };
} // namespace C8::Cipher
