#include "c8cpp-cipher/sha256.hpp"
#include "c8cpp-cipher/error.hpp"
#include "c8cpp-cipher/hex_encoder.hpp"

#include <openssl/evp.h>

namespace C8::Cipher {
  Sha256::Hash::Hash(CipherDataRO input) {
    value_.resize(EVP_MAX_MD_SIZE);

    OsslPointer<EVP_MD_CTX> evpMdCtx = OsslPointer<EVP_MD_CTX>::create();

    Error::checkSuccess(EVP_DigestInit_ex(evpMdCtx.raw(), EVP_sha256(), nullptr) == 1, "EVP_DigestInit_ex failed");
    Error::checkSuccess(EVP_DigestUpdate(evpMdCtx.raw(), input.asU8(), input.size()) == 1, "EVP_DigestUpdate failed");

    unsigned outputSize = 0;
    Error::checkSuccess(
      EVP_DigestFinal_ex(evpMdCtx.raw(), value_.view().asU8(), &outputSize) == 1, "EVP_DigestFinal_ex failed");

    value_.resize(outputSize);
  }

  Sha256::Hash::Hash(FromDump_t, std::string input) {
    HexEncoder::IO io(std::move(input));
    io >> value_;
  }

  Sha256::Hash::Hash(const Hash& other) {
    value_.append(other.value_.cview());
  }

  Sha256::Hash& Sha256::Hash::operator=(const Hash& other) {
    value_ = CipherData{};
    value_.append(other.value_.cview());
    return *this;
  }

  bool Sha256::Hash::operator==(const Hash& other) const {
    return value_.view() == other.value_.view();
  }

  CipherDataRO Sha256::Hash::value() const {
    return value_.view();
  }

  std::string Sha256::Hash::dump() const {
    return HexEncoder::encode(value_.view());
  }
} // namespace C8::Cipher
