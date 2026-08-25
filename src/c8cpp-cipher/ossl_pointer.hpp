#pragma once

#include "c8cpp-cipher/error.hpp"
#include "c8cpp-core/assert.hpp"

#include <utility>

namespace C8::Cipher {
  template <typename T>
  struct OsslPointerTraits;

  template <typename T>
  class OsslPointer : public OsslPointerTraits<T> {
  public:
    template <typename... Args>
    static OsslPointer create(Args&&... args)
    {
      const auto ptr = OsslPointerTraits<T>::create(std::forward<Args>(args)...);
      C8_ASSERTT(Cipher::Error::AssertError, ptr != nullptr, "Failed to allocate openssl resource.");
      return OsslPointer(ptr);
    }

    OsslPointer(const OsslPointer&) = delete;
    OsslPointer(OsslPointer&& other) : raw_(std::exchange(other.raw_, nullptr)) {}

    ~OsslPointer()
    {
      OsslPointerTraits<T>::destroy(raw_);
    }

    OsslPointer& operator=(const OsslPointer&) = delete;
    OsslPointer& operator=(OsslPointer&& other)
    {
      reset();

      raw_ = std::exchange(other.raw_, nullptr);

      return *this;
    }

    template <typename Self>
    auto& operator*(this Self& self)
    {
      return *self.raw_;
    }

    void reset()
    {
      OsslPointerTraits<T>::destroy(raw_);
      raw_ = nullptr;
    }

    explicit operator bool() const
    {
      return raw_ != nullptr;
    }

    template <typename Self>
    auto* raw(this Self& self)
    {
      return self.raw_;
    }

  private:
    explicit OsslPointer(T* raw) : raw_(raw) {}

    T* raw_ = nullptr;
  };
} // namespace C8::Cipher
