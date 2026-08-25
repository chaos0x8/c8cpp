#pragma once

#include "c8cpp-cipher/cipher_data.hpp"
#include "c8cpp-cipher/error.hpp"
#include "c8cpp-cipher/ossl_pointer.hpp"
#include "c8cpp-core/assert.hpp"
#include "c8cpp-core/utc_time.hpp"
#include "c8cpp-network/error.hpp"
#include "c8cpp-network/socket_coro.hpp"

#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>

#include <filesystem>
#include <vector>

namespace C8::Cipher {
  struct FromPublicKey_t {
  };
  struct FromPrivateKey_t {
  };
  struct FromFile_t {
  };
  struct FromRaw_t {
  };
  struct FromReference_t {
  };

  constexpr FromPublicKey_t fromPublicKey{};
  constexpr FromPrivateKey_t fromPrivateKey{};
  constexpr FromFile_t fromFile{};
  constexpr FromRaw_t fromRaw{};
  constexpr FromReference_t fromReference{};

  struct ServerMethod_t {
  };
  struct ClientMethod_t {
  };

  constexpr ServerMethod_t serverMethod;
  constexpr ClientMethod_t clientMethod;
} // namespace C8::Cipher
