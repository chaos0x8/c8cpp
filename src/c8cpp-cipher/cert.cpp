#include "c8cpp-cipher/cert.hpp"
#include "c8cpp-cipher/asymmetric_key.hpp"
#include "c8cpp-cipher/error.hpp"
#include "c8cpp-cipher/raii.hpp"
#include "c8cpp-cipher/utility.hpp"

#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>

#include <string>

namespace C8::Cipher {
  Cert::Cert(const IssuerInfo& issuer, KeySize keySize, std::chrono::seconds lifetime)
    : key_(keySize), x509_(OsslPointer<X509>::create()) {
    ASN1_INTEGER_set(X509_get_serialNumber(x509_.raw()), 1);
    X509_gmtime_adj(X509_get_notBefore(x509_.raw()), 0);
    X509_gmtime_adj(X509_get_notAfter(x509_.raw()), lifetime.count());
    X509_set_pubkey(x509_.raw(), key_.raw());

    X509_NAME* name = X509_get_subject_name(x509_.raw());
    X509_NAME_add_entry_by_txt(name, "C", MBSTRING_ASC, Utility::asU8(issuer.country.data()), -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "O", MBSTRING_ASC, Utility::asU8(issuer.organization.data()), -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, Utility::asU8(issuer.commonName.data()), -1, -1, 0);
    X509_set_issuer_name(x509_.raw(), name);

    X509_sign(x509_.raw(), key_.raw(), EVP_sha256());
  }

  Cert::Cert(CipherDataRO privateKey, CipherDataRO publicKey)
    : key_(fromPrivateKey, privateKey), x509_(OsslPointer<X509>::create(publicKey)) {}

  Cert::Cert(Cert&& other) : key_(std::move(other.key_)), x509_(std::move(other.x509_)) {}

  Cert& Cert::operator=(Cert&& other) {
    key_ = std::move(other.key_);
    x509_ = std::move(other.x509_);
    return *this;
  }

  Cert Cert::fromFile(const std::filesystem::path& privateKeyPath, const std::filesystem::path& publicKeyPath) {
    return Cert(Utility::readFile(privateKeyPath), Utility::readFile(publicKeyPath));
  }

  Cert Cert::clone() const {
    return Cert(privateKey(), publicKey());
  }

  void Cert::writePrivateKeyFile(const std::filesystem::path& path) const {
    auto keyFile = OsslPointer<FILE>::create(path, "wb");
    PEM_write_PrivateKey(keyFile.raw(), key_.raw(), nullptr, nullptr, 0, nullptr, nullptr);
  }

  void Cert::writePublicKeyFile(const std::filesystem::path& path) const {
    auto certFile = OsslPointer<FILE>::create(path, "wb");
    PEM_write_X509(certFile.raw(), x509_.raw());
  }

  CipherData Cert::privateKey() const {
    return key_.privateKey();
  }

  CipherData Cert::publicKey() const {
    auto bio = OsslPointer<BIO>::create();
    bio.write(x509_);
    return bio.get_mem_data();
  }

  C8::Core::UtcTime Cert::expirationDate() const {
    return x509_.expirationDate();
  }

  std::string Cert::commonName() const {
    return x509_.commonName();
  }
} // namespace C8::Cipher
