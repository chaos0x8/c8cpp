#include "c8cpp-cipher/cert_storage.hpp"

#include <fstream>

namespace C8::Cipher {
  template <typename PolicyTag>
  class CertStorage : public ICertStorage {
  public:
    CertStorage(std::optional<Cert> cert) : cert_(std::move(cert)) {}
    CertStorage(const CertStorage&) = delete;
    CertStorage(CertStorage&&) = delete;

    CertStorage& operator=(const CertStorage&) = delete;
    CertStorage& operator=(CertStorage&&) = delete;

    void authorize(OsslPointer<X509> cert) override {
      authorizedKeys_.emplace_back(std::move(cert));
    }

    void configure(OsslPointer<SSL_CTX>& ctx) override {
      if (cert_) {
        Error::checkSuccess(SSL_CTX_use_certificate(ctx.raw(), cert_->x509().raw()), "SSL_CTX_use_certificate failed");
        Error::checkSuccess(SSL_CTX_use_PrivateKey(ctx.raw(), cert_->evpPkey().raw()), "SSL_CTX_use_PrivateKey failed");
      }

      if (!authorizedKeys_.empty()) {
        X509_STORE* store = SSL_CTX_get_cert_store(ctx.raw());
        Error::checkSuccess(store != nullptr, "SSL_CTX_get_cert_store failed");

        for (const auto& x509 : authorizedKeys_) {
          Error::checkSuccess(X509_STORE_add_cert(store, x509.raw()) == 1, "X509_STORE_add_cert failed");
        }
      }

      setVerify_(ctx);
    }

    std::optional<CipherData> publicKey() const {
      if (cert_) {
        return cert_->publicKey();
      }

      return {};
    }

  private:
    void setVerify_(OsslPointer<SSL_CTX>& ctx) {
      if constexpr (std::is_same_v<PolicyTag, VerifyPeer::No_t>) {
        SSL_CTX_set_verify(ctx.raw(), SSL_VERIFY_PEER, nullptr);
      } else if constexpr (std::is_same_v<PolicyTag, VerifyPeer::Yes_t>) {
        SSL_CTX_set_verify(ctx.raw(), SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, nullptr);
      } else if constexpr (std::is_same_v<PolicyTag, void>) {
        if (!authorizedKeys_.empty()) {
          SSL_CTX_set_verify(ctx.raw(), SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, nullptr);
        } else {
          SSL_CTX_set_verify(ctx.raw(), SSL_VERIFY_PEER, nullptr);
        }
      } else {
        static_assert(std::is_same_v<PolicyTag, void>, "Invalid tag.");
      }
    }

    std::optional<Cert> cert_;
    std::vector<OsslPointer<X509>> authorizedKeys_;
  };

  std::unique_ptr<ICertStorage> createCertStorage(std::optional<Cert> cert) {
    return std::make_unique<CertStorage<void>>(std::move(cert));
  }

  std::unique_ptr<ICertStorage> createCertStorage(VerifyPeer::Yes_t, std::optional<Cert> cert) {
    return std::make_unique<CertStorage<VerifyPeer::Yes_t>>(std::move(cert));
  }

  std::unique_ptr<ICertStorage> createCertStorage(VerifyPeer::No_t, std::optional<Cert> cert) {
    return std::make_unique<CertStorage<VerifyPeer::No_t>>(std::move(cert));
  }
} // namespace C8::Cipher
