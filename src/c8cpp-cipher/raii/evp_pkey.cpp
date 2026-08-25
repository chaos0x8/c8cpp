#include "c8cpp-cipher/raii/evp_pkey.hpp"
#include "c8cpp-cipher/raii/bio.hpp"
#include "c8cpp-cipher/raii/x509.hpp"
#include "c8cpp-cipher/raii/x509_req.hpp"

namespace C8::Cipher {
  EVP_PKEY* OsslPointerTraits<EVP_PKEY>::create(EVP_PKEY_CTX& evpPkeyCtx)
  {
    EVP_PKEY* pkey = nullptr;
    EVP_PKEY_keygen(&evpPkeyCtx, &pkey);
    return pkey;
  }

  EVP_PKEY* OsslPointerTraits<EVP_PKEY>::create(FromPublicKey_t, CipherDataRO bytes)
  {
    auto bio = OsslPointer<BIO>::create(bytes);
    return PEM_read_bio_PUBKEY(bio.raw(), nullptr, nullptr, nullptr);
  }

  EVP_PKEY* OsslPointerTraits<EVP_PKEY>::create(FromPrivateKey_t, CipherDataRO bytes)
  {
    auto bio = OsslPointer<BIO>::create(bytes);
    return PEM_read_bio_PrivateKey(bio.raw(), nullptr, nullptr, nullptr);
  }

  EVP_PKEY* OsslPointerTraits<EVP_PKEY>::create(FromRaw_t, EVP_PKEY* raw)
  {
    return raw;
  }

  void OsslPointerTraits<EVP_PKEY>::destroy(EVP_PKEY* ptr)
  {
    EVP_PKEY_free(ptr);
  }

  CipherData OsslPointerTraits<EVP_PKEY>::get_mem_data() const
  {
    auto bio = OsslPointer<BIO>::create();
    bio.write(static_cast<const OsslPointer<EVP_PKEY>&>(*this));

    return bio.get_mem_data();
  }
} // namespace C8::Cipher
