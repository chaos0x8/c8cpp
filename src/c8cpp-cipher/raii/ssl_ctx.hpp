#pragma once

#include "c8cpp-cipher/raii/raii.hpp"

namespace C8::Cipher {
  template <>
  struct OsslPointerTraits<SSL_CTX> {
    static SSL_CTX* create(ServerMethod_t);
    static SSL_CTX* create(ClientMethod_t);
    static void destroy(SSL_CTX* ptr);

    virtual ~OsslPointerTraits() = default;

    void use_certificate(OsslPointer<X509>& x509);
    void use_PrivateKey(OsslPointer<EVP_PKEY>& pkey);
    void use_certificate_file(const std::string& file, int type);
    void use_PrivateKey_file(const std::string& file, int type);
    void load_verify_locations(const std::optional<std::string>& caFile, const std::optional<std::string>& caPath);
    void set_verify(int mode, SSL_verify_cb verifyCallback);
  };
} // namespace C8::Cipher
