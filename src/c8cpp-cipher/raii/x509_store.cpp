#include "c8cpp-cipher/raii/x509_store.hpp"

namespace C8::Cipher {
  X509_STORE* OsslPointerTraits<X509_STORE>::create()
  {
    return X509_STORE_new();
  }

  void OsslPointerTraits<X509_STORE>::destroy(X509_STORE* ptr)
  {
    X509_STORE_free(ptr);
  }
} // namespace C8::Cipher
