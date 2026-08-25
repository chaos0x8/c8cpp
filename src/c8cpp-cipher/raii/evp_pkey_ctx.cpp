#include "c8cpp-cipher/raii/evp_pkey_ctx.hpp"
#include "c8cpp-cipher/raii/evp_pkey.hpp"

namespace C8::Cipher {
  EVP_PKEY_CTX* OsslPointerTraits<EVP_PKEY_CTX>::create()
  {
    return EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);
  }

  EVP_PKEY_CTX* OsslPointerTraits<EVP_PKEY_CTX>::create(EVP_PKEY& evpPkey)
  {
    return EVP_PKEY_CTX_new(&evpPkey, nullptr);
  }

  void OsslPointerTraits<EVP_PKEY_CTX>::destroy(EVP_PKEY_CTX* ptr)
  {
    EVP_PKEY_CTX_free(ptr);
  }

  void OsslPointerTraits<EVP_PKEY_CTX>::keygen_init()
  {
    C8_ASSERTT(Cipher::Error::AssertError, 
      EVP_PKEY_keygen_init(static_cast<OsslPointer<EVP_PKEY_CTX>&>(*this).raw()) == 1, "EVP_PKEY_keygen_init failed");
  }

  void OsslPointerTraits<EVP_PKEY_CTX>::set_rsa_keygen_bits(int bits)
  {
    C8_ASSERTT(Cipher::Error::AssertError, EVP_PKEY_CTX_set_rsa_keygen_bits(static_cast<OsslPointer<EVP_PKEY_CTX>&>(*this).raw(), bits) == 1,
      "EVP_PKEY_CTX_set_rsa_keygen_bits failed");
  }

  OsslPointer<EVP_PKEY> OsslPointerTraits<EVP_PKEY_CTX>::keygen()
  {
    EVP_PKEY* pkey = nullptr;
    C8_ASSERTT(Cipher::Error::AssertError, 
      EVP_PKEY_keygen(static_cast<OsslPointer<EVP_PKEY_CTX>&>(*this).raw(), &pkey) == 1, "EVP_PKEY_keygen failed");
    return OsslPointer<EVP_PKEY>::create(fromRaw, pkey);
  }
} // namespace C8::Cipher
