#pragma once

#include "c8cpp-network/pipe.hpp"
#include "c8cpp-network/socket_coro.hpp"
#include "c8cpp-network/tcp_ip_socket.hpp"

#include <atomic>
#include <coroutine>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <thread>

namespace C8::Network {
  class SocketSelector {
  public:
    struct ISelectHandler {
      enum class Result { Noop, Done };

      virtual ~ISelectHandler() = default;

      virtual Result operator()() = 0;
    };

    template <typename T, typename... Args>
    struct SelectHandler : public ISelectHandler {
    private:
      template <typename F, size_t... Indexes>
      void invoke(F handler, std::index_sequence<Indexes...>)
      {
        coro_ = std::make_unique<SocketCoro<>>(handler(obj_, std::get<Indexes>(context_)...));
        coro_->resume();
      }

    public:
      template <typename F>
      SelectHandler(T obj, F handler, Args&&... args) : obj_(std::move(obj)), context_(std::forward<Args>(args)...)
      {
        invoke(std::move(handler), std::index_sequence_for<Args...>());
      }

      Result operator()() override
      {
        coro_->resume();

        if (!coro_->done()) {
          return Result::Noop;
        }

        coro_->value();
        return Result::Done;
      }

    private:
      T obj_;
      std::tuple<Args...> context_;
      std::unique_ptr<SocketCoro<>> coro_;
    };

    SocketSelector();
    ~SocketSelector();

    void start();
    void stop();

    template <typename T, typename F, typename... Args>
    void insert(T&& obj, F&& handler, Args&&... args)
    {
      std::unique_lock<std::mutex> lock{addQueueMutex_};

      const auto fd = obj.raw();

      addQueue_[fd] = std::make_unique<SelectHandler<T, Args...>>(
        std::forward<T>(obj), std::forward<F>(handler), std::forward<Args>(args)...);

      pipe_.w.writeString("A");
    }

    template <typename T>
    void remove(T& obj)
    {
      remove(obj.raw());
    }

  private:
    void epollAdd(FileDescriptor::value_type);
    void epollDel(FileDescriptor::value_type);
    void remove(FileDescriptor::value_type fd);

    void executeHandler(FileDescriptor::value_type);
    void processAddQueue();
    void processDelQueue();

    mutable std::mutex addQueueMutex_;
    mutable std::mutex delQueueMutex_;

    FileDescriptor epollFd_;
    Pipe pipe_{};
    std::thread t_;
    std::map<FileDescriptor::value_type, std::unique_ptr<ISelectHandler>> handlers_;
    std::map<FileDescriptor::value_type, std::unique_ptr<ISelectHandler>> addQueue_;
    std::vector<FileDescriptor::value_type> delQueue_;
    std::atomic<bool> started_ = false;
    std::atomic<bool> updated_ = false;
  };
} // namespace C8::Network
