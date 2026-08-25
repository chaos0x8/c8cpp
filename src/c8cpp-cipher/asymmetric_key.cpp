#include "c8cpp-cipher/asymmetric_key.hpp"
#include "c8cpp-cipher/error.hpp"

#include <openssl/pem.h>
#include <openssl/rsa.h>

namespace C8::Cipher {
  namespace {
    OsslPointer<EVP_PKEY> generatePkey(KeySize keySize) {
      auto evpPkeyCtx = OsslPointer<EVP_PKEY_CTX>::create();

      Error::checkSuccess(EVP_PKEY_keygen_init(evpPkeyCtx.raw()), "EVP_PKEY_keygen_init failed");
      Error::checkSuccess(EVP_PKEY_CTX_set_rsa_keygen_bits(evpPkeyCtx.raw(), std::to_underlying(keySize)),
        "EVP_PKEY_CTX_set_rsa_keygen_bits failed");

      return OsslPointer<EVP_PKEY>::create(*evpPkeyCtx);
    }
  } // namespace

  AsymmetricKey::AsymmetricKey(KeySize keySize) : evpPkey_(generatePkey(keySize)) {}

  AsymmetricKey::AsymmetricKey(FromPublicKey_t tag, CipherDataRO publicKey)
    : evpPkey_(OsslPointer<EVP_PKEY>::create(tag, publicKey)) {}

  AsymmetricKey::AsymmetricKey(FromPrivateKey_t tag, CipherDataRO privateKey)
    : evpPkey_(OsslPointer<EVP_PKEY>::create(tag, privateKey)) {}

  CipherData AsymmetricKey::encrypt(CipherDataRO input) {
    auto ctx = OsslPointer<EVP_PKEY_CTX>::create(*evpPkey_);
    Error::checkSuccess(EVP_PKEY_encrypt_init(ctx.raw()) > 0, "EVP_PKEY_encrypt_init failed");

    size_t outlen{};
    Error::checkSuccess(
      EVP_PKEY_encrypt(ctx.raw(), nullptr, &outlen, input.asU8(), input.size()) > 0, "EVP_PKEY_encrypt failed");

    CipherData output(outlen);
    Error::checkSuccess(EVP_PKEY_encrypt(ctx.raw(), output.view().asU8(), &outlen, input.asU8(), input.size()) > 0,
      "EVP_PKEY_encrypt failed");

    output.resize(outlen);
    return output;
  }

  CipherData AsymmetricKey::decrypt(CipherDataRO input) {
    auto ctx = OsslPointer<EVP_PKEY_CTX>::create(*evpPkey_);
    Error::checkSuccess(EVP_PKEY_decrypt_init(ctx.raw()) > 0, "EVP_PKEY_encrypt_init failed");

    size_t outlen{};
    Error::checkSuccess(
      EVP_PKEY_decrypt(ctx.raw(), nullptr, &outlen, input.asU8(), input.size()) > 0, "EVP_PKEY_encrypt failed");

    CipherData output(outlen);
    Error::checkSuccess(EVP_PKEY_decrypt(ctx.raw(), output.view().asU8(), &outlen, input.asU8(), input.size()) > 0,
      "EVP_PKEY_decrypt failed");

    output.resize(outlen);
    return output;
  }

  CipherData AsymmetricKey::publicKey() const {
    auto bio = OsslPointer<BIO>::create();
    Error::checkSuccess(PEM_write_bio_PUBKEY(bio.raw(), evpPkey_.raw()) == 1, "PEM_write_bio_PUBKEY failed");

    char* data = nullptr;
    long len = BIO_get_mem_data(bio.raw(), &data);
    std::span<const std::byte> bytes = std::as_bytes(std::span<char>(data, data + len));
    return std::vector<std::byte>(bytes.begin(), bytes.end());
  }

  CipherData AsymmetricKey::privateKey() const {
    auto bio = OsslPointer<BIO>::create();
    bio.write(evpPkey_);
    return bio.get_mem_data();
  }
} // namespace C8::Cipher
