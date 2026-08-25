#include "c8cpp-network/pipe.hpp"
#include "c8cpp-network/error.hpp"

#include <fcntl.h>
#include <unistd.h>

#include <array>

namespace C8::Network {
  ReadPipe::ReadPipe(FileDescriptor fd) : fd_(std::move(fd)){};

  SocketCoro<std::string> ReadPipe::readString()
  {
    std::string buffer(128, '\0');
    const auto nread = co_await read(buffer.data(), buffer.size());
    buffer.resize(nread);
    co_return buffer;
  }

  SocketCoro<size_t> ReadPipe::read(void* data, size_t size)
  {
    while (true) {
      const ssize_t nread{::read(fd_.raw(), data, size)};
      if (nread < 0) {
        if (errno == EAGAIN) {
          co_await std::suspend_always{};
          continue;
        }
      } else if (nread > 0) {
        co_return static_cast<size_t>(nread);
      }

      break;
    }

    throw Error::SystemError(errno);
  }

  FileDescriptor::value_type ReadPipe::raw()
  {
    return fd_.raw();
  }

  WritePipe::WritePipe(FileDescriptor fd) : fd_(std::move(fd)){};

  void WritePipe::writeString(std::string_view buffer)
  {
    write(reinterpret_cast<const void*>(buffer.data()), buffer.size());
  }

  void WritePipe::write(const void* data, size_t size)
  {
    while (true) {
      const ssize_t nwrite{::write(fd_.raw(), data, size)};
      if (nwrite < 0) {
        if (errno == EAGAIN) {
          continue;
        }
      } else if (static_cast<size_t>(nwrite) == size) {
        return;
      }

      break;
    }

    throw Error::SystemError(errno);
  }

  FileDescriptor::value_type WritePipe::raw()
  {
    return fd_.raw();
  }

  Pipe createPipe()
  {
    std::array<int, 2> fds{};
    if (::pipe2(fds.data(), O_NONBLOCK) < 0) {
      throw Error::SystemError(errno);
    }

    return Pipe{.r = ReadPipe(FileDescriptor(fds[0])), .w = WritePipe(FileDescriptor(fds[1]))};
  }
} // namespace C8::Network
