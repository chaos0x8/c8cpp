#pragma once

#include "c8cpp-cipher/cipher_data.hpp"
#include "c8cpp-cipher/hex_encoder.hpp"
#include "c8cpp-cipher/tag.hpp"

#include <cstdint>
#include <span>
#include <string_view>
#include <vector>

namespace C8::Cipher {
  class Pbkdf2 {
  public:
    struct Config {
      bool operator==(const Config&) const = default;

      void enc(HexEncoder::IO&) const;
      void dec(HexEncoder::IO&);

      uint64_t saltSize = 32;
      uint64_t keySize = 256;
      uint32_t iterations = 100'000u;
    };

    struct Hash {
      explicit Hash(Pbkdf2::Config, CipherDataRO secret, CipherDataRO salt);
      explicit Hash(Pbkdf2::Config, CipherDataRO secret);
      explicit Hash(FromDump_t, std::string);
      Hash(const Hash&);
      Hash(Hash&&) = default;

      Hash& operator=(const Hash&);
      Hash& operator=(Hash&&) = default;

      bool operator==(const Hash&) const;
      bool operator==(const CipherDataRO&) const;
      bool operator!=(const Hash&) const;
      bool operator!=(const CipherDataRO&) const;

      Pbkdf2::Config config() const;
      CipherDataRO salt() const;
      CipherDataRO key() const;
      std::string dump() const;

    private:
      Pbkdf2::Config config_;
      CipherData value_;
    };
  };
} // namespace C8::Cipher
