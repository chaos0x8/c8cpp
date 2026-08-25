#pragma once

#include "c8cpp-cipher/cipher_data.hpp"
#include "c8cpp-cipher/raii.hpp"
#include "c8cpp-cipher/utility.hpp"

#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>

#include <cstddef>
#include <memory>
#include <vector>

namespace C8::Cipher {
  enum class KeySize { k2 = 2048, k4 = 4096, k8 = 8192 };

  class AsymmetricKey {
  public:
    explicit AsymmetricKey(KeySize);
    explicit AsymmetricKey(FromPublicKey_t, CipherDataRO publicKey);
    explicit AsymmetricKey(FromPrivateKey_t, CipherDataRO privateKey);

    CipherData encrypt(CipherDataRO input);
    CipherData decrypt(CipherDataRO input);

    CipherData publicKey() const;
    CipherData privateKey() const;

    const OsslPointer<EVP_PKEY>& evpPkey() const {
      return evpPkey_;
    }

    template <typename Self>
    auto raw(this Self& self) {
      return self.evpPkey_.raw();
    }

  private:
    OsslPointer<EVP_PKEY> evpPkey_;
  };
} // namespace C8::Cipher
