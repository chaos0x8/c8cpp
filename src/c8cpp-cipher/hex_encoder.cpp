#include "c8cpp-cipher/hex_encoder.hpp"

#include <iomanip>
#include <sstream>

namespace C8::Cipher {
  namespace Detail {
    std::string encode(std::span<const std::byte> input) {
      std::stringstream ss;
      for (const std::byte ch : input) {
        ss << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(ch);
      }
      return ss.str();
    }

    void decode(std::string_view input, std::span<std::byte> output) {
      if (input.size() % 2 != 0) {
        throw std::runtime_error("Invalid hex");
      }

      if (input.size() != output.size() * 2) {
        throw std::runtime_error("Decode IO error");
      }

      for (size_t i = 0; i < input.size(); i += 2) {
        const std::string byteString(reinterpret_cast<const char*>(input.data()) + i, 2);
        output[i / 2] = static_cast<std::byte>(std::stoi(byteString, nullptr, 16));
      }
    }
  } // namespace Detail

  std::string HexEncoder::encode(const CipherDataRO& view) {
    return Detail::encode(view.span());
  }

  CipherData HexEncoder::decode(std::string_view input) {
    CipherData out{input.size() / 2};
    Detail::decode(input, out.view().span());
    return out;
  }

  HexEncoder::IO::IO(std::string init) : hex_(std::move(init)) {}

  HexEncoder::IO& HexEncoder::IO::operator<<(const CipherDataRO& in) {
    hex_ += Detail::encode(in.span());
    return *this;
  }

  HexEncoder::IO& HexEncoder::IO::operator>>(CipherDataRW& out) {
    std::string_view input{reinterpret_cast<const char*>(hex_.data()), std::min(hex_.size(), out.size() * 2)};
    Detail::decode(input, out.span());
    hex_ = hex_.substr(input.size());
    return *this;
  }

  HexEncoder::IO& HexEncoder::IO::operator>>(CipherData& out) {
    out.resize(hex_.size() / 2);
    CipherDataRW view = out.view();
    (*this) >> view;
    return *this;
  }
} // namespace C8::Cipher
