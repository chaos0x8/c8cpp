#include "c8cpp-cipher/raii/bio.hpp"
#include "c8cpp-cipher/raii/evp_pkey.hpp"
#include "c8cpp-cipher/raii/x509.hpp"
#include "c8cpp-cipher/raii/x509_req.hpp"

namespace C8::Cipher {
  BIO* OsslPointerTraits<BIO>::create()
  {
    return BIO_new(BIO_s_mem());
  }

  BIO* OsslPointerTraits<BIO>::create(CipherDataRO bytes)
  {
    return BIO_new_mem_buf(bytes.data(), bytes.size());
  }

  BIO* OsslPointerTraits<BIO>::create(FromFile_t, const std::filesystem::path& path)
  {
    return BIO_new_file(path.c_str(), "r");
  }

  void OsslPointerTraits<BIO>::destroy(BIO* ptr)
  {
    BIO_free(ptr);
  }

  OsslPointer<X509_REQ> OsslPointerTraits<BIO>::read_X509_REQ()
  {
    return OsslPointer<X509_REQ>::create(
      fromRaw, PEM_read_bio_X509_REQ(static_cast<OsslPointer<BIO>&>(*this).raw(), nullptr, nullptr, nullptr));
  }

  OsslPointer<X509> OsslPointerTraits<BIO>::read_X509()
  {
    return OsslPointer<X509>::create(
      fromRaw, PEM_read_bio_X509(static_cast<OsslPointer<BIO>&>(*this).raw(), nullptr, nullptr, nullptr));
  }

  OsslPointer<EVP_PKEY> OsslPointerTraits<BIO>::read_PrivateKey()
  {
    return OsslPointer<EVP_PKEY>::create(
      fromRaw, PEM_read_bio_PrivateKey(static_cast<OsslPointer<BIO>&>(*this).raw(), nullptr, nullptr, nullptr));
  }

  void OsslPointerTraits<BIO>::write(const OsslPointer<X509>& x509)
  {
    C8_ASSERTT(Cipher::Error::AssertError,
      PEM_write_bio_X509(static_cast<OsslPointer<BIO>&>(*this).raw(), x509.raw()) == 1, "PEM_write_bio_X509 failed");
  }

  void OsslPointerTraits<BIO>::write(const OsslPointer<EVP_PKEY>& evpPkey)
  {
    C8_ASSERTT(Cipher::Error::AssertError,
      PEM_write_bio_PKCS8PrivateKey(
        static_cast<OsslPointer<BIO>&>(*this).raw(), evpPkey.raw(), NULL, NULL, 0, NULL, NULL) == 1,
      "PEM_write_bio_PKCS8PrivateKey failed");
  }

  void OsslPointerTraits<BIO>::write(const OsslPointer<X509_REQ>& x509Req)
  {
    C8_ASSERTT(Cipher::Error::AssertError,
      PEM_write_bio_X509_REQ(static_cast<OsslPointer<BIO>&>(*this).raw(), x509Req.raw()) == 1,
      "PEM_write_bio_X509_REQ failed");
  }

  CipherData OsslPointerTraits<BIO>::get_mem_data() const
  {
    char* data = nullptr;
    long len = BIO_get_mem_data(static_cast<const OsslPointer<BIO>&>(*this).raw(), &data);
    std::span<const std::byte> bytes = std::as_bytes(std::span<char>(data, data + len));
    return std::vector<std::byte>(bytes.begin(), bytes.end());
  }
} // namespace C8::Cipher
