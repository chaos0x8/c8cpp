#include "c8cpp-cipher/raii/evp_cipher_ctx.hpp"

namespace C8::Cipher {
  EVP_CIPHER_CTX* OsslPointerTraits<EVP_CIPHER_CTX>::create()
  {
    return EVP_CIPHER_CTX_new();
  }

  void OsslPointerTraits<EVP_CIPHER_CTX>::destroy(EVP_CIPHER_CTX* ptr)
  {
    EVP_CIPHER_CTX_free(ptr);
  }
} // namespace C8::Cipher
