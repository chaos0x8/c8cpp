#include "c8cpp-cipher/utility.hpp"
#include "c8cpp-cipher/error.hpp"

#include <openssl/rand.h>

#include <fstream>

namespace C8::Cipher::Utility {
  std::vector<std::byte> readFile(const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) {
      throw Error::FileError::readError(path);
    }

    const size_t size = std::filesystem::file_size(path);
    if (size == 0) {
      throw Error::FileError::readError(path);
    }

    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
      throw Error::FileError::readError(path);
    }

    std::vector<std::byte> output(size);
    file.read(asChar(output.data()), output.size());

    return output;
  }

  CipherData randomBytes(size_t size) {
    CipherData result(size);

    Error::checkSuccess(RAND_bytes(result.view().asU8(), result.view().size()), "Failed to generate salt");

    return result;
  }

  std::string toString(std::span<const std::byte> bytes) {
    return std::string(reinterpret_cast<const char*>(bytes.data()), bytes.size());
  }

  std::string_view asString(std::span<const std::byte> bytes) {
    return std::string_view(reinterpret_cast<const char*>(bytes.data()), bytes.size());
  }
} // namespace C8::Cipher::Utility
