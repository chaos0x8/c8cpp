#pragma once

#include "c8cpp-cipher/cipher_data.hpp"

#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace C8::Cipher {
  // More or less secure string
  class MolsString {
  public:
    MolsString() = default;
    MolsString(const CipherDataRO&);
    MolsString(const MolsString&) = delete;
    MolsString(MolsString&&) = delete;
    ~MolsString();

    MolsString& operator=(const CipherDataRO&);
    MolsString& operator=(const MolsString&) = delete;
    MolsString& operator=(MolsString&&) = delete;

    std::unique_ptr<MolsString> clone();

    CipherDataRO value() const;

  private:
    void purge();
    void assign(CipherDataRO);

    static constexpr size_t passwordSize_ = 32;

    CipherData password_;
    CipherData data_;
    mutable CipherData tmp_;
  };
} // namespace C8::Cipher
