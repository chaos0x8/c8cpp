#include "c8cpp-cipher/raii/file.hpp"

namespace C8::Cipher {
  FILE* OsslPointerTraits<FILE>::create(const std::filesystem::path& path, const std::string& mode)
  {
    return fopen(path.c_str(), mode.c_str());
  }

  void OsslPointerTraits<FILE>::destroy(FILE* ptr)
  {
    fclose(ptr);
  }
} // namespace C8::Cipher
