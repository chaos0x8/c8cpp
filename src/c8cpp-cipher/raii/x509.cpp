#include "c8cpp-cipher/raii/x509.hpp"
#include "c8cpp-cipher/raii/bio.hpp"
#include "c8cpp-cipher/raii/evp_pkey.hpp"
#include "c8cpp-cipher/raii/file.hpp"
#include "c8cpp-cipher/raii/x509_name.hpp"

namespace C8::Cipher {
  X509* OsslPointerTraits<X509>::create()
  {
    return X509_new();
  }

  X509* OsslPointerTraits<X509>::create(FromFile_t, const std::filesystem::path& path)
  {
    auto file = OsslPointer<FILE>::create(path, "rb");
    return PEM_read_X509(file.raw(), nullptr, nullptr, nullptr);
  }

  X509* OsslPointerTraits<X509>::create(FromRaw_t, X509* raw)
  {
    return raw;
  }

  X509* OsslPointerTraits<X509>::create(CipherDataRO bytes)
  {
    auto bio = OsslPointer<BIO>::create(bytes);
    return PEM_read_bio_X509(bio.raw(), nullptr, nullptr, nullptr);
  }

  void OsslPointerTraits<X509>::destroy(X509* ptr)
  {
    X509_free(ptr);
  }

  void OsslPointerTraits<X509>::set_version(long version)
  {
    C8_ASSERTT(Cipher::Error::AssertError, X509_set_version(static_cast<OsslPointer<X509>&>(*this).raw(), version) == 1, "X509_set_version failed");
  }

  void OsslPointerTraits<X509>::set_subject_name(const OsslPointer<X509_NAME>& name)
  {
    C8_ASSERTT(Cipher::Error::AssertError, X509_set_subject_name(static_cast<OsslPointer<X509>&>(*this).raw(), name.raw()) == 1,
      "X509_set_subject_name failed");
  }

  void OsslPointerTraits<X509>::set_issuer_name(const OsslPointer<X509_NAME>& name)
  {
    C8_ASSERTT(Cipher::Error::AssertError, X509_set_issuer_name(static_cast<OsslPointer<X509>&>(*this).raw(), name.raw()) == 1,
      "X509_set_issuer_name failed");
  }

  void OsslPointerTraits<X509>::set_pubkey(const OsslPointer<EVP_PKEY>& pkey)
  {
    C8_ASSERTT(Cipher::Error::AssertError, X509_set_pubkey(static_cast<OsslPointer<X509>&>(*this).raw(), pkey.raw()) == 1, "X509_set_pubkey failed");
  }

  OsslPointer<X509_NAME> OsslPointerTraits<X509>::get_subject_name() const
  {
    return OsslPointer<X509_NAME>::create(
      fromReference, X509_get_subject_name(static_cast<const OsslPointer<X509>&>(*this).raw()));
  }

  OsslPointer<EVP_PKEY> OsslPointerTraits<X509>::get_pubkey() const
  {
    return OsslPointer<EVP_PKEY>::create(fromRaw, X509_get_pubkey(static_cast<const OsslPointer<X509>&>(*this).raw()));
  }

  std::string OsslPointerTraits<X509>::asn1ExpirationDate() const
  {
    const auto& self = static_cast<const OsslPointer<X509>&>(*this);

    const auto notAfter = X509_get_notAfter(self.raw());
    auto bio = OsslPointer<BIO>::create();
    ASN1_TIME_print(bio.raw(), notAfter);
    return bio.get_mem_data().string();
  }

  C8::Core::UtcTime OsslPointerTraits<X509>::expirationDate() const
  {
    const auto& self = static_cast<const OsslPointer<X509>&>(*this);

    std::tm tm_utc{};
    std::istringstream ss(self.asn1ExpirationDate());
    ss >> std::get_time(&tm_utc, "%b %d %H:%M:%S %Y");
    Error::checkSuccess(!ss.fail(), "Parsing asn1 time failed.");
    std::stringstream out;
    out << std::put_time(&tm_utc, "%F %T");
    return C8::Core::UtcTime(out.str());
  }

  std::string OsslPointerTraits<X509>::commonName() const
  {
    const auto& self = static_cast<const OsslPointer<X509>&>(*this);

    X509_NAME* x509Name = X509_get_subject_name(self.raw());
    C8_ASSERTT(Cipher::Error::AssertError, x509Name, "Failed to read Name.");

    const int index = X509_NAME_get_index_by_NID(x509Name, NID_commonName, -1);
    Error::checkSuccess(index != -1, "Failed to find commonName in Name.");

    const X509_NAME_ENTRY* entry = X509_NAME_get_entry(x509Name, index);
    Error::checkSuccess(entry, "Failed to read commonName entry.");

    ASN1_STRING* entryData = X509_NAME_ENTRY_get_data(entry);
    Error::checkSuccess(entryData, "Failed to read commonName entry data.");

    auto bio = OsslPointer<BIO>::create();
    ASN1_STRING_print(bio.raw(), entryData);

    return bio.get_mem_data().string();
  }
} // namespace C8::Cipher
