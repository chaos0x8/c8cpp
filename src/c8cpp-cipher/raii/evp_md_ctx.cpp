#include "c8cpp-cipher/raii/evp_md_ctx.hpp"

namespace C8::Cipher {
  EVP_MD_CTX* OsslPointerTraits<EVP_MD_CTX>::create()
  {
    return EVP_MD_CTX_new();
  }

  void OsslPointerTraits<EVP_MD_CTX>::destroy(EVP_MD_CTX* ptr)
  {
    EVP_MD_CTX_free(ptr);
  }
} // namespace C8::Cipher
