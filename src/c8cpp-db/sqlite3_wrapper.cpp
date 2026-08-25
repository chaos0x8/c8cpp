#include "c8cpp-db/sqlite3_wrapper.hpp"

namespace C8::Db {
  namespace Detail {
    void SqLite3DbDeleter::operator()(sqlite3* ptr) const {
      if (ptr) {
        sqlite3_close(ptr);
      }
    }
  } // namespace Detail

  SqLite3Wrapper::SqLite3Wrapper(std::filesystem::path path) : path_(std::move(path)) {}

  void SqLite3Wrapper::setInMomeryJournal() {
    executeImpl("PRAGMA synchronous = OFF; PRAGMA journal_mode = MEMORY;", Statement::Params{});
  }

  std::filesystem::path SqLite3Wrapper::path() const {
    return path_;
  }

  Detail::SqLite3DbPointer SqLite3Wrapper::openDb(std::filesystem::path path) {
    sqlite3* tmpDb = nullptr;
    const int err = sqlite3_open(path.c_str(), &tmpDb);
    Detail::SqLite3DbPointer result(tmpDb);
    if (!result || err != SQLITE_OK) {
      throw Error::StandardError(std::format("Cannot open db `{}', error code = {}", path.c_str(), err));
    }
    return result;
  }
} // namespace C8::Db
