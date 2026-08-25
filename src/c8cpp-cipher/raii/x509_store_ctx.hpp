#pragma once

#include "c8cpp-cipher/raii/raii.hpp"

namespace C8::Cipher {
  template <>
  struct OsslPointerTraits<X509_STORE_CTX> {
    static X509_STORE_CTX* create();
    static void destroy(X509_STORE_CTX* ptr);

    virtual ~OsslPointerTraits() = default;
  };
} // namespace C8::Cipher
