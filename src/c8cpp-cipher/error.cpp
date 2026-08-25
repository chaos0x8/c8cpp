#include "c8cpp-cipher/error.hpp"

#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>

#include <format>

namespace C8::Cipher::Error {
  namespace {
    struct BioDeleter {
      void operator()(BIO* ptr)
      {
        BIO_free(ptr);
      }
    };

    std::string getOsslErrorString()
    {
      std::unique_ptr<BIO, BioDeleter> bio(BIO_new(BIO_s_mem()));

      if (!bio) {
        return "Failed to allocate error string buffer";
      }

      ERR_print_errors(bio.get());

      char* data = nullptr;
      long len = BIO_get_mem_data(bio.get(), &data);

      if (data && len > 0) {
        return {data, static_cast<size_t>(len)};
      }

      return "Failed to retrieve error string";
    }
  } // namespace

  StandardError::StandardError(std::string_view message) : Error(std::format("Cipher::StandardError: {}", message)) {}

  AssertError::AssertError(std::string_view message)
    : Error(std::format("Cipher::AssertError: {}, [{}]", message, getOsslErrorString()))
  {
  }

  FileError::FileError(std::string_view message) : Error(std::format("Cipher::FileError: {}", message)) {}

  FileError FileError::readError(const std::filesystem::path& path)
  {
    return FileError(std::format("Could not read from file \"{}\"", path.c_str()));
  }

  ConnectionRefusedError::ConnectionRefusedError(std::string_view message)
    : Error(std::format("Cipher::ConnectionRefusedError: {} []", message, getOsslErrorString()))
  {
  }

  ConnectionRefusedError ConnectionRefusedError::acceptError()
  {
    return ConnectionRefusedError("Accept failed.");
  }

  ConnectionRefusedError ConnectionRefusedError::connectError()
  {
    return ConnectionRefusedError("Connect failed.");
  }
} // namespace C8::Cipher::Error
