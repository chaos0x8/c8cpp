#pragma once

#include "c8cpp-network/file_descriptor.hpp"
#include "c8cpp-network/socket_coro.hpp"

#include <span>

namespace C8::Network {
  class ReadPipe {
  public:
    ReadPipe() = default;
    explicit ReadPipe(FileDescriptor);

    SocketCoro<std::string> readString();
    SocketCoro<size_t> read(void*, size_t);

    FileDescriptor::value_type raw();

  private:
    FileDescriptor fd_;
  };

  class WritePipe {
  public:
    WritePipe() = default;
    explicit WritePipe(FileDescriptor);

    void writeString(std::string_view);
    void write(const void*, size_t);

    FileDescriptor::value_type raw();

  private:
    FileDescriptor fd_;
  };

  struct Pipe {
    ReadPipe r;
    WritePipe w;
  };

  Pipe createPipe();
} // namespace C8::Network
