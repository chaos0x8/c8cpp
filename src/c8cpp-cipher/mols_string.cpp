#include "c8cpp-cipher/mols_string.hpp"
#include "c8cpp-cipher/aes_cipher.hpp"
#include "c8cpp-cipher/utility.hpp"

namespace C8::Cipher {
  MolsString::MolsString(const CipherDataRO& input) {
    assign(input);
  }

  MolsString::~MolsString() {
    purge();
  }

  MolsString& MolsString::operator=(const CipherDataRO& input) {
    assign(input);
    return *this;
  }

  CipherDataRO MolsString::value() const {
    tmp_ = AesCipher::decrypt(data_, password_);
    return tmp_.cview();
  }

  void MolsString::purge() {
    password_.purge();
    data_.purge();
    tmp_.purge();
  }

  void MolsString::assign(CipherDataRO input) {
    password_ = Utility::randomBytes(passwordSize_);
    data_ = AesCipher::encrypt(input, password_.cview());
    tmp_.purge();
  }

  std::unique_ptr<MolsString> MolsString::clone() {
    auto cloned = std::make_unique<MolsString>(value());
    tmp_.purge();
    return cloned;
  }
} // namespace C8::Cipher
