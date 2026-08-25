#include "c8cpp-cipher/aes_cipher.hpp"
#include "c8cpp-cipher/error.hpp"
#include "c8cpp-cipher/pbkdf2.hpp"
#include "c8cpp-cipher/sha256.hpp"

#include <openssl/aes.h>

#include <utility>

namespace C8::Cipher {
  namespace {
    static constexpr size_t ivSize = AES_BLOCK_SIZE;
    static constexpr size_t keySize = 32;
    static constexpr size_t saltSize = 16;
    static constexpr unsigned iterations = 1'000u;
  } // namespace

  CipherData AesCipher::encrypt(CipherDataRO input, CipherDataRO password) {
    OsslPointer<EVP_CIPHER_CTX> evpCipherCtx = OsslPointer<EVP_CIPHER_CTX>::create();

    const auto hash =
      Pbkdf2::Hash({.saltSize = saltSize, .keySize = ivSize + keySize, .iterations = iterations}, password);
    const auto iv = hash.key().subview(0, ivSize);
    const auto key = hash.key().subview(ivSize);

    Error::checkSuccess(EVP_EncryptInit_ex(evpCipherCtx.raw(), EVP_aes_256_cbc(), nullptr, key.asU8(), iv.asU8()),
      "Failed to initialize encryption");

    const auto salt = hash.salt();
    CipherData output;
    output.append(salt);
    output.resize(salt.size() + input.size() + EVP_CIPHER_block_size(EVP_aes_256_cbc()));
    size_t totalOutputLength = salt.size();
    int len{};
    Error::checkSuccess(
      EVP_EncryptUpdate(evpCipherCtx.raw(), output.view().asU8() + totalOutputLength, &len, input.asU8(), input.size()),
      "Failed to encrypt data");
    totalOutputLength += len;

    Error::checkSuccess(EVP_EncryptFinal_ex(evpCipherCtx.raw(), output.view().asU8() + totalOutputLength, &len),
      "Failed to finalize encryption");
    totalOutputLength += len;

    output.resize(totalOutputLength);
    return output;
  }

  CipherData AesCipher::decrypt(CipherDataRO input, CipherDataRO password) {
    OsslPointer<EVP_CIPHER_CTX> evpCipherCtx = OsslPointer<EVP_CIPHER_CTX>::create();

    const auto salt = input.subview(0, saltSize);
    const auto hash =
      Pbkdf2::Hash({.saltSize = saltSize, .keySize = ivSize + keySize, .iterations = iterations}, password, salt);
    const auto iv = hash.key().subview(0, ivSize);
    const auto key = hash.key().subview(ivSize);

    Error::checkSuccess(EVP_DecryptInit_ex(evpCipherCtx.raw(), EVP_aes_256_cbc(), nullptr, key.asU8(), iv.asU8()),
      "Failed to initialize decryption");

    size_t totalOutputLength = 0;
    int len{};
    CipherData output(input.size() + EVP_CIPHER_block_size(EVP_aes_256_cbc()));
    Error::checkSuccess(EVP_DecryptUpdate(evpCipherCtx.raw(), output.view().asU8() + totalOutputLength, &len,
                          input.asU8() + saltSize, input.size() - saltSize),
      "Failed to decrypt data");
    totalOutputLength += len;

    Error::checkSuccess(EVP_DecryptFinal_ex(evpCipherCtx.raw(), output.view().asU8() + totalOutputLength, &len),
      "Failed to finalize decryption");
    totalOutputLength += len;

    output.resize(totalOutputLength);
    return output;
  }
} // namespace C8::Cipher
