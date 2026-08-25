#include "c8cpp-cipher/raii/ssl_ctx.hpp"
#include "c8cpp-cipher/raii/evp_pkey.hpp"
#include "c8cpp-cipher/raii/x509.hpp"

namespace C8::Cipher {
  SSL_CTX* OsslPointerTraits<SSL_CTX>::create(ServerMethod_t)
  {
    return SSL_CTX_new(TLS_server_method());
  }

  SSL_CTX* OsslPointerTraits<SSL_CTX>::create(ClientMethod_t)
  {
    return SSL_CTX_new(TLS_client_method());
  }

  void OsslPointerTraits<SSL_CTX>::destroy(SSL_CTX* ptr)
  {
    SSL_CTX_free(ptr);
  }

  void OsslPointerTraits<SSL_CTX>::use_certificate(OsslPointer<X509>& x509)
  {
    C8_ASSERTT(Cipher::Error::AssertError, SSL_CTX_use_certificate(static_cast<OsslPointer<SSL_CTX>&>(*this).raw(), x509.raw()) == 1,
      "SSL_CTX_use_certificate failed");
  }

  void OsslPointerTraits<SSL_CTX>::use_PrivateKey(OsslPointer<EVP_PKEY>& pkey)
  {
    C8_ASSERTT(Cipher::Error::AssertError, SSL_CTX_use_PrivateKey(static_cast<OsslPointer<SSL_CTX>&>(*this).raw(), pkey.raw()) == 1,
      "SSL_CTX_use_PrivateKey failed");
  }

  void OsslPointerTraits<SSL_CTX>::use_certificate_file(const std::string& file, int type)
  {
    C8_ASSERTT(Cipher::Error::AssertError, SSL_CTX_use_certificate_file(static_cast<OsslPointer<SSL_CTX>&>(*this).raw(), file.c_str(), type) == 1,
      "SSL_CTX_use_certificate_file failed");
  }

  void OsslPointerTraits<SSL_CTX>::use_PrivateKey_file(const std::string& file, int type)
  {
    C8_ASSERTT(Cipher::Error::AssertError, SSL_CTX_use_PrivateKey_file(static_cast<OsslPointer<SSL_CTX>&>(*this).raw(), file.c_str(), type) == 1,
      "SSL_CTX_use_PrivateKey_file failed");
  }

  void OsslPointerTraits<SSL_CTX>::load_verify_locations(
    const std::optional<std::string>& caFile, const std::optional<std::string>& caPath)
  {
    C8_ASSERTT(Cipher::Error::AssertError, SSL_CTX_load_verify_locations(static_cast<OsslPointer<SSL_CTX>&>(*this).raw(),
                caFile.transform([](auto& str) { return str.c_str(); }).value_or(nullptr),
                caPath.transform([](auto& str) { return str.c_str(); }).value_or(nullptr)) == 1,
      "SSL_CTX_load_verify_locations failed");
  }

  void OsslPointerTraits<SSL_CTX>::set_verify(int mode, SSL_verify_cb verifyCallback)
  {
    SSL_CTX_set_verify(static_cast<OsslPointer<SSL_CTX>&>(*this).raw(), mode, verifyCallback);
  }
} // namespace C8::Cipher
