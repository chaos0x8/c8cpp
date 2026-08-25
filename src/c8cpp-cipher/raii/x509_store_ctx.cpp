#include "c8cpp-cipher/raii/x509_store_ctx.hpp"

namespace C8::Cipher {
  X509_STORE_CTX* OsslPointerTraits<X509_STORE_CTX>::create()
  {
    return X509_STORE_CTX_new();
  }

  void OsslPointerTraits<X509_STORE_CTX>::destroy(X509_STORE_CTX* ptr)
  {
    X509_STORE_CTX_free(ptr);
  }
} // namespace C8::Cipher
