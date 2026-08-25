#pragma once

#include "c8cpp-cipher/raii/raii.hpp"

namespace C8::Cipher {
  template <>
  struct OsslPointerTraits<X509_NAME> {
    static X509_NAME* create(FromRaw_t, X509_NAME* raw);
    static X509_NAME* create(FromReference_t, X509_NAME* raw);
    static void destroy(X509_NAME* ptr);

    virtual ~OsslPointerTraits() = default;

    void print(OsslPointer<BIO>& bio) const;
    void add_entry_by_txt(const std::string& field, const std::string& value);

    std::string toString() const;
  };
} // namespace C8::Cipher
