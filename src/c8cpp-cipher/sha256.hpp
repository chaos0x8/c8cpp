#pragma once

#include "c8cpp-cipher/cipher_data.hpp"
#include "c8cpp-cipher/raii.hpp"
#include "c8cpp-cipher/tag.hpp"

#include <openssl/evp.h>

#include <memory>
#include <span>
#include <string_view>
#include <vector>

namespace C8::Cipher {
  class Sha256 {
  public:
    struct Hash {
      explicit Hash(CipherDataRO);
      explicit Hash(FromDump_t, std::string);
      Hash(const Hash&);
      Hash(Hash&&) = default;

      Hash& operator=(const Hash&);
      Hash& operator=(Hash&&) = default;

      bool operator==(const Hash&) const;

      CipherDataRO value() const;

      std::string dump() const;

    private:
      CipherData value_;
    };
  };
} // namespace C8::Cipher
