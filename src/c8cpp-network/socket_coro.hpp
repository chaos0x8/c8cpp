#pragma once

#include <coroutine>
#include <exception>
#include <iostream>
#include <memory>
#include <optional>
#include <utility>

namespace C8::Network {
  struct CoroHandle {
    struct IHandle {
      virtual ~IHandle() = default;

      virtual std::coroutine_handle<> last() = 0;
      virtual operator std::coroutine_handle<>() = 0;
    };

    template <typename T>
    struct HandleT : public IHandle {
      explicit HandleT(std::coroutine_handle<T> coro) : coro_(std::move(coro)) {}

      std::coroutine_handle<> last() override
      {
        if (coro_ && coro_.promise().next_) {
          if (auto next = coro_.promise().next_.last()) {
            return next;
          }
        }

        return coro_;
      }

      operator std::coroutine_handle<>() override
      {
        return coro_;
      }

      std::coroutine_handle<T> coro_{nullptr};
    };

    template <typename T>
    CoroHandle& operator=(std::coroutine_handle<T> h)
    {
      handle_ = std::make_unique<HandleT<T>>(std::move(h));
      return *this;
    }

    CoroHandle& operator=(std::nullptr_t)
    {
      handle_ = nullptr;
      return *this;
    }

    std::coroutine_handle<> last()
    {
      if (handle_) {
        return handle_->last();
      }

      return {nullptr};
    }

    operator std::coroutine_handle<>()
    {
      return *handle_;
    }

    explicit operator bool() const
    {
      return static_cast<bool>(handle_);
    }

  private:
    std::unique_ptr<IHandle> handle_;
  };

  template <typename T = void>
  struct SocketCoro;

  template <typename T>
  struct FinalSuspendAwaiter {
    bool await_ready() noexcept
    {
      return false;
    }

    std::coroutine_handle<> await_suspend(std::coroutine_handle<T> h) noexcept
    {
      if (h.promise().prev_) {
        return h.promise().prev_;
      } else {
        return std::noop_coroutine();
      }
    }

    void await_resume() noexcept {}
  };

  template <typename T>
  struct SocketCoroAwaiter {
    SocketCoroAwaiter(SocketCoro<T> coro) : coro_(std::move(coro)) {}

    bool await_ready() noexcept
    {
      return false;
    }

    template <typename U>
    auto await_suspend(std::coroutine_handle<U> h)
    {
      h.promise().next_ = coro_.h_;
      coro_.h_.promise().prev_ = std::move(h);
      return coro_.h_;
    }

    T await_resume()
    {
      if (coro_.h_.promise().exception_) {
        std::rethrow_exception(coro_.h_.promise().exception_);
      }

      if constexpr (!std::is_same_v<T, void>) {
        return std::move(*coro_.h_.promise().value_);
      } else {
        return;
      }
    }

    SocketCoro<T> coro_;
  };

  template <typename T>
  struct SuspendAlwaysAwaiter {
    bool await_ready() noexcept
    {
      return false;
    }

    void await_suspend(std::coroutine_handle<T> h) noexcept
    {
      h.promise().next_ = nullptr;
    }

    void await_resume() noexcept {}
  };

  template <typename T>
  struct SocketCoro {
    using value_type = T;

    struct promise_type {
      SocketCoro get_return_object()
      {
        return SocketCoro(std::coroutine_handle<promise_type>::from_promise(*this));
      }

      std::suspend_always initial_suspend() noexcept
      {
        return {};
      }

      auto final_suspend() noexcept
      {
        return FinalSuspendAwaiter<promise_type>{};
      }

      template <typename U>
      auto await_transform(SocketCoro<U> other)
      {
        return SocketCoroAwaiter(std::move(other));
      }

      auto await_transform(std::suspend_always)
      {
        return SuspendAlwaysAwaiter<promise_type>{};
      }

      void unhandled_exception()
      {
        exception_ = std::current_exception();
      }

      void return_value(T value)
      {
        value_ = std::move(value);
      }

      std::optional<T> value_;
      std::exception_ptr exception_;
      CoroHandle prev_{};
      CoroHandle next_{};
    };

    SocketCoro(std::coroutine_handle<promise_type> h) : h_(h) {}
    SocketCoro(const SocketCoro&) = delete;
    SocketCoro(SocketCoro&& other) : h_(std::exchange(other.h_, nullptr)) {}

    SocketCoro& operator=(const SocketCoro&) = delete;
    SocketCoro& operator=(SocketCoro&& other)
    {
      h_.destroy();
      h_ = std::exchange(other.h_, nullptr);
      return *this;
    }

    ~SocketCoro()
    {
      if (h_) {
        h_.destroy();
      }
    }

    void resume()
    {
      if (h_.done()) {
        return;
      }

      if (auto next = h_.promise().next_.last(); next) {
        next.resume();
        return;
      }

      h_.resume();
    }

    bool done() const
    {
      return h_.done();
    }

    T value()
    {
      if (h_.promise().exception_) {
        std::rethrow_exception(h_.promise().exception_);
      }

      return std::move(*h_.promise().value_);
    }

    T resolve()
    {
      while (!done()) {
        resume();
      }

      return value();
    }

    std::coroutine_handle<promise_type> h_;
  };

  template <>
  struct SocketCoro<void> {
    using value_type = void;

    struct promise_type {
      SocketCoro get_return_object()
      {
        return SocketCoro(std::coroutine_handle<promise_type>::from_promise(*this));
      }

      std::suspend_always initial_suspend() noexcept
      {
        return {};
      }

      auto final_suspend() noexcept
      {
        return FinalSuspendAwaiter<promise_type>{};
      }

      template <typename U>
      auto await_transform(SocketCoro<U> other)
      {
        return SocketCoroAwaiter(std::move(other));
      }

      auto await_transform(std::suspend_always)
      {
        return SuspendAlwaysAwaiter<promise_type>{};
      }

      void unhandled_exception()
      {
        exception_ = std::current_exception();
      }

      void return_void() {}

      std::exception_ptr exception_;
      CoroHandle prev_;
      CoroHandle next_;
    };

    SocketCoro(std::coroutine_handle<promise_type> h) : h_(h) {}
    SocketCoro(const SocketCoro&) = delete;
    SocketCoro(SocketCoro&& other) : h_(std::exchange(other.h_, nullptr)) {}

    SocketCoro& operator=(const SocketCoro&) = delete;
    SocketCoro& operator=(SocketCoro&& other)
    {
      h_.destroy();
      h_ = std::exchange(other.h_, nullptr);
      return *this;
    }

    ~SocketCoro()
    {
      if (h_) {
        h_.destroy();
      }
    }

    void resume()
    {
      if (h_.done()) {
        return;
      }

      if (auto next = h_.promise().next_.last(); next) {
        next.resume();
        return;
      }

      h_.resume();
    }

    bool done() const
    {
      return h_.done();
    }

    void value()
    {
      if (h_.promise().exception_) {
        std::rethrow_exception(h_.promise().exception_);
      }
    }

    void resolve()
    {
      while (!done()) {
        resume();
      }

      return value();
    }

    std::coroutine_handle<promise_type> h_;
  };
} // namespace C8::Network
