#include "c8cpp-cipher/global_init.hpp"

#include <openssl/err.h>
#include <openssl/ssl.h>

namespace C8::Cipher::Detail {
  void OsslErrorStrings::init() {
    SSL_load_error_strings();
  }

  void OsslErrorStrings::free() {
    ERR_free_strings();
  }

  void OsslLibrary::init() {
    SSL_library_init();
  }
} // namespace C8::Cipher::Detail
