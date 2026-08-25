#pragma once

#include "c8cpp-cipher/raii/raii.hpp"

namespace C8::Cipher {
  template <>
  struct OsslPointerTraits<X509_STORE> {
    static X509_STORE* create();
    static void destroy(X509_STORE* ptr);

    virtual ~OsslPointerTraits() = default;
  };
} // namespace C8::Cipher
