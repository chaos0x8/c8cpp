#include "c8cpp-cipher/raii/x509_name.hpp"
#include "c8cpp-cipher/raii/bio.hpp"

namespace C8::Cipher {
  X509_NAME* OsslPointerTraits<X509_NAME>::create(FromRaw_t, X509_NAME* raw)
  {
    return raw;
  }

  X509_NAME* OsslPointerTraits<X509_NAME>::create(FromReference_t, X509_NAME* raw)
  {
    return X509_NAME_dup(raw);
  }

  void OsslPointerTraits<X509_NAME>::destroy(X509_NAME* ptr)
  {
    X509_NAME_free(ptr);
  }

  void OsslPointerTraits<X509_NAME>::print(OsslPointer<BIO>& bio) const
  {
    C8_ASSERTT(Cipher::Error::AssertError, X509_NAME_print(bio.raw(), static_cast<const OsslPointer<X509_NAME>&>(*this).raw(), 0) == 1,
      "X509_NAME_print failed");
  }

  void OsslPointerTraits<X509_NAME>::add_entry_by_txt(const std::string& field, const std::string& value)
  {
    C8_ASSERTT(Cipher::Error::AssertError, X509_NAME_add_entry_by_txt(static_cast<OsslPointer<X509_NAME>&>(*this).raw(), field.c_str(), MBSTRING_ASC,
                reinterpret_cast<const unsigned char*>(value.c_str()), -1, -1, 0) == 1,
      "X509_NAME_add_entry_by_txt failed");
  }

  std::string OsslPointerTraits<X509_NAME>::toString() const
  {
    auto bio = OsslPointer<BIO>::create();
    print(bio);
    return bio.get_mem_data().string();
  }
} // namespace C8::Cipher
