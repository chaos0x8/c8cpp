#pragma once

#include "c8cpp-cipher/raii/raii.hpp"

namespace C8::Cipher {
  template <>
  struct OsslPointerTraits<BIO> {
    static BIO* create();
    static BIO* create(CipherDataRO bytes);
    static BIO* create(FromFile_t, const std::filesystem::path& path);
    static void destroy(BIO* ptr);

    virtual ~OsslPointerTraits() = default;

    OsslPointer<X509_REQ> read_X509_REQ();
    OsslPointer<X509> read_X509();
    OsslPointer<EVP_PKEY> read_PrivateKey();

    void write(const OsslPointer<X509>&);
    void write(const OsslPointer<EVP_PKEY>&);
    void write(const OsslPointer<X509_REQ>&);
    CipherData get_mem_data() const;
  };
} // namespace C8::Cipher
