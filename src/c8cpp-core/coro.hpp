#pragma once

#include <coroutine>
#include <exception>
#include <ranges>
#include <utility>

namespace C8::Core {
  template <typename T>
  class Generator : public std::ranges::view_base {
  public:
    struct promise_type {
      Generator get_return_object() {
        return Generator(std::coroutine_handle<promise_type>::from_promise(*this));
      }

      std::suspend_always initial_suspend() {
        return {};
      }
      std::suspend_always final_suspend() noexcept {
        return {};
      };
      void unhandled_exception() {
        exception_ = std::current_exception();
      }
      std::suspend_always yield_value(T value) {
        value_ = std::move(value);
        return {};
      }
      void return_void() {};

      T value_{};
      std::exception_ptr exception_;
    };

    Generator(std::coroutine_handle<promise_type> h) : h_(h) {}

    ~Generator() {
      h_.destroy();
    }

    bool next() {
      if (h_.done()) {
        return false;
      }

      h_.resume();

      if (h_.promise().exception_) {
        std::rethrow_exception(h_.promise().exception_);
      }

      return !h_.done();
    }

    const T& value() const {
      return h_.promise().value_;
    }

  private:
    std::coroutine_handle<promise_type> h_;
  };

  template <typename T = void>
  class Task {
  public:
    struct promise_type {
      Task get_return_object() {
        return Task(std::coroutine_handle<promise_type>::from_promise(*this));
      }

      std::suspend_always initial_suspend() {
        return {};
      }
      std::suspend_always final_suspend() noexcept {
        return {};
      };
      void unhandled_exception() {
        exception_ = std::current_exception();
      }
      void return_value(T value) {
        value_ = std::move(value);
      };

      T value_{};
      std::exception_ptr exception_;
    };

    Task(std::coroutine_handle<promise_type> h) : h_(h) {}

    ~Task() {
      h_.destroy();
    }

    void resume() {
      if (h_.done()) {
        return;
      }

      h_.resume();
    }

    bool done() const {
      return h_.done();
    }

    T value() const {
      if (h_.promise().exception_) {
        std::rethrow_exception(h_.promise().exception_);
      }

      return h_.promise().value_;
    }

  private:
    std::coroutine_handle<promise_type> h_;
  };

  template <>
  class Task<void> {
  public:
    struct promise_type {
      Task get_return_object() {
        return Task(std::coroutine_handle<promise_type>::from_promise(*this));
      }

      std::suspend_always initial_suspend() {
        return {};
      }
      std::suspend_always final_suspend() noexcept {
        return {};
      };
      void unhandled_exception() {
        exception_ = std::current_exception();
      }
      void return_void() {};

      std::exception_ptr exception_;
    };

    Task(std::coroutine_handle<promise_type> h) : h_(h) {}

    ~Task() {
      h_.destroy();
    }

    void resume() {
      if (h_.done()) {
        return;
      }

      h_.resume();
    }

    bool done() const {
      return h_.done();
    }

    void value() const {
      if (h_.promise().exception_) {
        std::rethrow_exception(h_.promise().exception_);
      }
    }

  private:
    std::coroutine_handle<promise_type> h_;
  };
} // namespace C8::Core
