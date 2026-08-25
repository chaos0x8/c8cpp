#pragma once

#include <cstddef>
#include <iostream>
#include <stdexcept>

namespace C8::Cipher {
  namespace Detail {
    struct OsslErrorStrings {
      static void init();
      static void free();
    };

    struct OsslLibrary {
      static void init();
    };

    template <typename T>
    concept Init = requires(T t)
    {
      t.init();
    };

    template <typename T>
    concept Free = requires(T t)
    {
      t.free();
    };

    template <typename T>
    concept InitOnly = Init<T> && !Free<T>;

    template <typename T>
    concept FreeAfterInit = Init<T>&& Free<T>;

    template <typename T>
    struct GlobalInitImpl;

    template <Detail::InitOnly T>
    class GlobalInitImpl<T> {
    public:
      GlobalInitImpl()
      {
        if (!initialized_) {
          T::init();
          initialized_ = true;
        }
      }

      GlobalInitImpl(const GlobalInitImpl&) = delete;
      GlobalInitImpl(GlobalInitImpl&&) = delete;

      GlobalInitImpl& operator=(const GlobalInitImpl&) = delete;
      GlobalInitImpl& operator=(GlobalInitImpl&&) = delete;

    private:
      static bool initialized_;
    };

    template <Detail::InitOnly T>
    bool GlobalInitImpl<T>::initialized_ = false;

    template <Detail::FreeAfterInit T>
    class GlobalInitImpl<T> {
    public:
      GlobalInitImpl()
      {
        if (refCounter_ == 0) {
          T::init();
        }

        ++refCounter_;
      }

      ~GlobalInitImpl()
      {
        if (refCounter_ == 0) {
          std::cerr << "GlobalInitImpl logic error.\n";
        }

        if (--refCounter_ == 0) {
          T::free();
        }
      }

      GlobalInitImpl(const GlobalInitImpl&) = delete;
      GlobalInitImpl(GlobalInitImpl&&) = delete;

      GlobalInitImpl& operator=(const GlobalInitImpl&) = delete;
      GlobalInitImpl& operator=(GlobalInitImpl&&) = delete;

    private:
      static size_t refCounter_;
    };

    template <Detail::FreeAfterInit T>
    size_t GlobalInitImpl<T>::refCounter_ = 0u;
  } // namespace Detail

  struct GlobalInit {
    Detail::GlobalInitImpl<Detail::OsslErrorStrings> errors;
    Detail::GlobalInitImpl<Detail::OsslLibrary> library;
  };
} // namespace C8::Cipher
