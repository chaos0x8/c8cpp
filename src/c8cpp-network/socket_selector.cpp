#include "c8cpp-network/socket_selector.hpp"
#include "c8cpp-core/logger.hpp"
#include "c8cpp-network/error.hpp"

#include <sys/epoll.h>

#include <csignal>

namespace C8::Network {
  SocketSelector::SocketSelector() {}

  SocketSelector::~SocketSelector()
  {
    stop();
  }

  void SocketSelector::start()
  {
    if (started_) {
      return;
    }

    epollFd_ = FileDescriptor{epoll_create1(0)};

    if (!epollFd_) {
      throw Error::SystemError(errno);
    }

    pipe_ = createPipe();

    insert(
      std::move(pipe_.r),
      [](ReadPipe& r, SocketSelector* selector) -> SocketCoro<> {
        while (true) {
          const auto str = co_await r.readString();

          if (str.find("A") != std::string::npos) {
            selector->processAddQueue();
          }

          if (str.find("D") != std::string::npos) {
            selector->processDelQueue();
          }
        }
      },
      this);

    started_ = true;

    t_ = std::thread([this] {
      std::signal(SIGPIPE, SIG_IGN);

      processAddQueue();
      processDelQueue();

      std::array<epoll_event, 32> events{};

      while (started_) {
        int numFds = epoll_wait(epollFd_.raw(), events.data(), events.size(), -1);
        if (numFds < 0) {
          return;
        }

        updated_ = false;

        for (int i = 0; i < numFds; ++i) {
          if (updated_) {
            break;
          }

          if (events[i].events & EPOLLIN) {
            if (events[i].data.fd == FileDescriptor::InvalidValue) {
              C8_CRITICAL("SocketSelector Invalid File Descriptor");
              stop();
              break;
            }

            executeHandler(events[i].data.fd);
          }
        }
      }
    });
  }

  void SocketSelector::stop()
  {
    if (!started_) {
      return;
    }

    started_ = false;

    pipe_.w.writeString("S");

    if (t_.joinable()) {
      t_.join();
    }

    handlers_.clear();
    addQueue_.clear();
    delQueue_.clear();
  }

  void SocketSelector::remove(FileDescriptor::value_type fd)
  {
    std::unique_lock<std::mutex> lock{delQueueMutex_};

    delQueue_.emplace_back(fd);

    pipe_.w.writeString("D");
  }

  void SocketSelector::epollAdd(FileDescriptor::value_type fd)
  {
    epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = fd;
    if (epoll_ctl(epollFd_.raw(), EPOLL_CTL_ADD, fd, &event) == -1) {
      throw Error::SystemError(errno);
    }
  }

  void SocketSelector::epollDel(FileDescriptor::value_type fd)
  {
    epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = fd;
    if (epoll_ctl(epollFd_.raw(), EPOLL_CTL_DEL, fd, &event) == -1) {
      throw Error::SystemError(errno);
    }
  }

  void SocketSelector::executeHandler(FileDescriptor::value_type fd)
  {
    try {
      auto handlerIt = handlers_.find(fd);
      if (handlerIt == handlers_.end()) {
        epollDel(fd);
        return;
      }

      switch ((*handlerIt->second)()) {
        case ISelectHandler::Result::Noop:
          break;
        case ISelectHandler::Result::Done:
          remove(fd);
          break;
      }
    } catch (const Error::IoError& e) {
      if (e.err() == 0) {
        remove(fd);
      } else {
        remove(fd);
        C8_CRITICAL("{}", e.what());
      }
    } catch (const std::exception& e) {
      remove(fd);
      C8_CRITICAL("{}", e.what());
    }
  }

  void SocketSelector::processAddQueue()
  {
    decltype(addQueue_) localAddQueue;
    {
      std::unique_lock<std::mutex> lock{addQueueMutex_};
      std::swap(localAddQueue, addQueue_);
    }

    for (auto& [fd, handler] : localAddQueue) {
      handlers_[fd] = std::move(handler);
      executeHandler(fd);
      epollAdd(fd);
      updated_ = true;
    }
  }

  void SocketSelector::processDelQueue()
  {
    decltype(delQueue_) localDelQueue;
    {
      std::unique_lock<std::mutex> lock{delQueueMutex_};
      std::swap(localDelQueue, delQueue_);
    }

    for (auto fd : localDelQueue) {
      if (auto it = handlers_.find(fd); it != handlers_.end()) {
        epollDel(fd);
        handlers_.erase(it);
        updated_ = true;
      }
    }
  }
} // namespace C8::Network
