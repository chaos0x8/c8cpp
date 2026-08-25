#pragma once

#include "c8cpp-cipher/raii/raii.hpp"

namespace C8::Cipher {
  template <>
  struct OsslPointerTraits<X509_REQ> {
    static X509_REQ* create();
    static X509_REQ* create(FromRaw_t, X509_REQ* raw);
    static void destroy(X509_REQ* ptr);

    virtual ~OsslPointerTraits() = default;

    void sign(OsslPointer<EVP_PKEY>& pkey);
    void set_version(int version);
    void set_pubkey(OsslPointer<EVP_PKEY>& pkey);
    void set_subject_name(OsslPointer<X509_NAME>& name);
    OsslPointer<EVP_PKEY> get_pubkey() const;
    OsslPointer<X509_NAME> get_subject_name() const;
  };
} // namespace C8::Cipher
