#pragma once

#include "c8cpp-cipher/cert.hpp"
#include "c8cpp-cipher/raii.hpp"

#include <optional>

namespace C8::Cipher {
  namespace VerifyPeer {
    struct Yes_t {};
    struct No_t {};

    constexpr auto yes = Yes_t{};
    constexpr auto no = No_t{};
  }; // namespace VerifyPeer

  class ICertStorage {
  public:
    virtual ~ICertStorage() = default;

    virtual void authorize(OsslPointer<X509> cert) = 0;
    virtual void configure(OsslPointer<SSL_CTX>&) = 0;

    virtual std::optional<CipherData> publicKey() const = 0;
  };

  std::unique_ptr<ICertStorage> createCertStorage(std::optional<Cert>);
  std::unique_ptr<ICertStorage> createCertStorage(VerifyPeer::Yes_t, std::optional<Cert>);
  std::unique_ptr<ICertStorage> createCertStorage(VerifyPeer::No_t, std::optional<Cert>);
} // namespace C8::Cipher
