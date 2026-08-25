#pragma once

#include "c8cpp-cipher/asymmetric_key.hpp"
#include "c8cpp-cipher/cipher_data.hpp"
#include "c8cpp-core/utc_time.hpp"

#include <chrono>
#include <filesystem>
#include <string>

namespace C8::Cipher {
  struct IssuerInfo {
    std::string country = "PL";
    std::string organization = "SelfSigned";
    std::string commonName = "SelfSigned.com";
  };

  class Cert {
  public:
    explicit Cert(const IssuerInfo& issuer, KeySize, std::chrono::seconds lifetime = std::chrono::years(1));
    explicit Cert(CipherDataRO privateKey, CipherDataRO publicKey);
    Cert(const Cert&) = delete;
    Cert(Cert&&);

    Cert& operator=(const Cert&) = delete;
    Cert& operator=(Cert&&);

    static Cert fromFile(const std::filesystem::path& privateKeyPath, const std::filesystem::path& publicKeyPath);

    Cert clone() const;

    void writePrivateKeyFile(const std::filesystem::path& path) const;
    void writePublicKeyFile(const std::filesystem::path& path) const;

    CipherData privateKey() const;
    CipherData publicKey() const;

    C8::Core::UtcTime expirationDate() const;
    std::string commonName() const;

    const OsslPointer<EVP_PKEY>& evpPkey() const {
      return key_.evpPkey();
    }

    const OsslPointer<X509>& x509() const {
      return x509_;
    }

  private:
    AsymmetricKey key_;
    OsslPointer<X509> x509_;
  };
} // namespace C8::Cipher
