#include "c8cpp-cipher/pbkdf2.hpp"
#include "c8cpp-cipher/error.hpp"
#include "c8cpp-cipher/hex_encoder.hpp"
#include "c8cpp-cipher/utility.hpp"

#include <openssl/evp.h>

namespace C8::Cipher {
  HexEncoder::IO& operator<<(HexEncoder::IO& io, const Pbkdf2::Config& config) {
    CipherDataRO view(std::as_bytes(std::span{&config, 1}));
    return io << view;
  }

  HexEncoder::IO& operator>>(HexEncoder::IO& io, Pbkdf2::Config& config) {
    CipherDataRW view(std::as_writable_bytes(std::span{&config, 1}));
    return io >> view;
  }

  Pbkdf2::Hash::Hash(Pbkdf2::Config config, CipherDataRO secret, CipherDataRO salt) : config_(config) {
    config_.saltSize = std::min(config_.saltSize, salt.size());

    value_.append(salt);
    value_.resize(salt.size() + config_.keySize);

    Error::checkSuccess(PKCS5_PBKDF2_HMAC(secret.asChar(), secret.size(), salt.asU8(), salt.size(), config_.iterations,
                          EVP_sha256(), config_.keySize, value_.view().asU8() + config_.saltSize),
      "PKCS5_PBKDF2_HMAC failed");
  }

  Pbkdf2::Hash::Hash(Pbkdf2::Config config, CipherDataRO secret)
    : Hash(config, secret, Utility::randomBytes(config.saltSize).cview()) {}

  Pbkdf2::Hash::Hash(FromDump_t, std::string input) {
    HexEncoder::IO io{std::move(input)};
    io >> config_;
    io >> value_;
  }

  Pbkdf2::Hash::Hash(const Hash& other) : config_(other.config_) {
    value_.append(other.value_.cview());
  }

  Pbkdf2::Hash& Pbkdf2::Hash::operator=(const Hash& other) {
    config_ = other.config_;
    value_ = CipherData{};
    value_.append(other.value_.cview());
    return *this;
  }

  bool Pbkdf2::Hash::operator==(const Hash& other) const {
    return config_ == other.config_ && value_.view() == other.value_.view();
  }

  bool Pbkdf2::Hash::operator==(const CipherDataRO& other) const {
    auto otherHash = Hash(config_, other, salt());
    return (*this) == otherHash;
  }

  bool Pbkdf2::Hash::operator!=(const Hash& other) const {
    return !operator==(other);
  }

  bool Pbkdf2::Hash::operator!=(const CipherDataRO& other) const {
    return !operator==(other);
  }

  CipherDataRO Pbkdf2::Hash::salt() const {
    return value_.cview().subview(0, config_.saltSize);
  }

  CipherDataRO Pbkdf2::Hash::key() const {
    return value_.cview().subview(config_.saltSize);
  }

  Pbkdf2::Config Pbkdf2::Hash::config() const {
    return config_;
  }

  std::string Pbkdf2::Hash::dump() const {
    HexEncoder::IO io{};
    io << config_;
    io << value_.cview();
    return io.takeHex();
  }
} // namespace C8::Cipher
