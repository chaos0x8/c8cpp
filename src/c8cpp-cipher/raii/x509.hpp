#pragma once

#include "c8cpp-cipher/raii/raii.hpp"

namespace C8::Cipher {
  template <>
  struct OsslPointerTraits<X509> {
    static X509* create();
    static X509* create(FromFile_t, const std::filesystem::path& path);
    static X509* create(FromRaw_t, X509* raw);
    static X509* create(CipherDataRO bytes);
    static void destroy(X509* ptr);

    virtual ~OsslPointerTraits() = default;

    void set_version(long version);
    void set_subject_name(const OsslPointer<X509_NAME>& name);
    void set_issuer_name(const OsslPointer<X509_NAME>& name);
    void set_pubkey(const OsslPointer<EVP_PKEY>& pkey);
    OsslPointer<X509_NAME> get_subject_name() const;
    OsslPointer<EVP_PKEY> get_pubkey() const;

    C8::Core::UtcTime expirationDate() const;
    std::string commonName() const;

  private:
    std::string asn1ExpirationDate() const;
  };
} // namespace C8::Cipher
