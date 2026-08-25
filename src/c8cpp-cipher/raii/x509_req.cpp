#include "c8cpp-cipher/raii/x509_req.hpp"
#include "c8cpp-cipher/raii/evp_pkey.hpp"
#include "c8cpp-cipher/raii/x509_name.hpp"

namespace C8::Cipher {
  X509_REQ* OsslPointerTraits<X509_REQ>::create()
  {
    return X509_REQ_new();
  }

  X509_REQ* OsslPointerTraits<X509_REQ>::create(FromRaw_t, X509_REQ* raw)
  {
    return raw;
  }

  void OsslPointerTraits<X509_REQ>::destroy(X509_REQ* ptr)
  {
    X509_REQ_free(ptr);
  }

  void OsslPointerTraits<X509_REQ>::sign(OsslPointer<EVP_PKEY>& pkey)
  {
    C8_ASSERTT(Cipher::Error::AssertError,
      X509_REQ_sign(static_cast<OsslPointer<X509_REQ>&>(*this).raw(), pkey.raw(), EVP_sha256()) > 0,
      "X509_REQ_sign failed");
  }

  void OsslPointerTraits<X509_REQ>::set_version(int version)
  {
    C8_ASSERTT(Cipher::Error::AssertError,
      X509_REQ_set_version(static_cast<OsslPointer<X509_REQ>&>(*this).raw(), version) == 1,
      "X509_REQ_set_version failed");
  }

  void OsslPointerTraits<X509_REQ>::set_pubkey(OsslPointer<EVP_PKEY>& pkey)
  {
    C8_ASSERTT(Cipher::Error::AssertError,
      X509_REQ_set_pubkey(static_cast<OsslPointer<X509_REQ>&>(*this).raw(), pkey.raw()) == 1,
      "X509_REQ_set_pubkey failed");
  }

  void OsslPointerTraits<X509_REQ>::set_subject_name(OsslPointer<X509_NAME>& name)
  {
    C8_ASSERTT(Cipher::Error::AssertError,
      X509_REQ_set_subject_name(static_cast<OsslPointer<X509_REQ>&>(*this).raw(), name.raw()) == 1,
      "X509_REQ_set_subject_name failed");
  }

  OsslPointer<EVP_PKEY> OsslPointerTraits<X509_REQ>::get_pubkey() const
  {
    return OsslPointer<EVP_PKEY>::create(
      fromRaw, X509_REQ_get_pubkey(static_cast<const OsslPointer<X509_REQ>&>(*this).raw()));
  }

  OsslPointer<X509_NAME> OsslPointerTraits<X509_REQ>::get_subject_name() const
  {
    return OsslPointer<X509_NAME>::create(
      fromReference, X509_REQ_get_subject_name(static_cast<const OsslPointer<X509_REQ>&>(*this).raw()));
  }
} // namespace C8::Cipher
