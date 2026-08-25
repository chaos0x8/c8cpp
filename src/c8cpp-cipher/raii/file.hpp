#pragma once

#include "c8cpp-cipher/raii/raii.hpp"

namespace C8::Cipher {
  template <>
  struct OsslPointerTraits<FILE> {
    static FILE* create(const std::filesystem::path& path, const std::string& mode);
    static void destroy(FILE* ptr);

    virtual ~OsslPointerTraits() = default;
  };
} // namespace C8::Cipher
