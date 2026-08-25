#include "c8cpp-cipher/raii/raii.hpp"

namespace C8::Cipher {
  template <>
  struct OsslPointerTraits<EVP_CIPHER_CTX> {
    static EVP_CIPHER_CTX* create();
    static void destroy(EVP_CIPHER_CTX* ptr);

    virtual ~OsslPointerTraits() = default;
  };
} // namespace C8::Cipher
