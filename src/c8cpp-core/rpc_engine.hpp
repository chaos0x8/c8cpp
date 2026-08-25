#pragma once

#include "c8cpp-core/assert.hpp"
#include "c8cpp-core/logger.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <expected>
#include <format>
#include <functional>
#include <meta>
#include <ranges>
#include <span>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <unordered_map>

namespace C8::Core {
  template <typename T>
  struct IsSpan : public std::false_type {
  };

  template <typename T>
  struct IsSpan<std::span<T>> : public std::true_type {
  };

  template <typename T>
  struct IsSpan<std::span<const T>> : public std::true_type {
  };

  template <typename T>
  concept Trivial = std::is_standard_layout_v<T>&& std::is_trivially_constructible_v<T>&&
    std::is_trivially_copyable_v<T>&& std::is_trivially_destructible_v<T>;

  template <typename T>
  concept Span = IsSpan<T>::value;

  template <typename T>
  concept SpanConvertible = requires(T t)
  {
    std::span(t);
  };

  template <typename T>
  consteval std::string_view id()
  {
    return std::meta::display_string_of(std::meta::dealias(^^T));
  }

  template <typename... Args>
  consteval std::string_view join()
  {
    if constexpr (sizeof...(Args) == 0) {
      return std::define_static_string("");
    } else {
      std::string r;
      size_t idx = 0;

      template for (constexpr auto info : std::define_static_array(std::array{^^Args...}))
      {
        if (idx++ != 0) {
          r += ", ";
        }

        using U = typename[:info:];
        r += id<U>();
      }
      return std::define_static_string(r);
    }
  }

  template <typename T>
  void encode(std::vector<std::byte>& out, const T& val)
  {
    if constexpr (std::is_pointer_v<T>) {
      static_assert(false, std::format("Pointer types are not supported. T = {}", id<T>()));
    } else if constexpr (Span<T>) {
      if constexpr (Trivial<typename T::value_type>) {
        const auto src = std::as_bytes(val);
        const auto offset = out.size();
        out.resize(out.size() + src.size());
        std::ranges::copy_n(src.data(), src.size(), std::span{out}.subspan(offset).data());
      } else {
        for (const auto& elem : val) {
          encode(out, elem);
        }
      }
    } else if constexpr (Trivial<T>) {
      encode(out, std::span{&val, 1});
    } else if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>) {
      encode(out, val.size());
      encode(out, std::span{val.data(), val.size()});
    } else if constexpr (SpanConvertible<T>) {
      encode(out, val.size());
      encode(out, std::span{val});
    } else {
      template for (constexpr auto member :
        std::define_static_array(std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::current())))
      {
        encode(out, val.[:member:]);
      }
    }
  }

  template <typename T>
  std::span<const std::byte> decode(T& out, std::span<const std::byte> payload)
  {
    if constexpr (std::is_pointer_v<T>) {
      static_assert(false, std::format("Pointer types are not supported. T = {}", id<T>()));
    } else if constexpr (Span<T>) {
      if constexpr (Trivial<typename T::value_type>) {
        auto dst = std::as_writable_bytes(out);
        C8_ASSERT(payload.size() >= dst.size(), "Unexpected end of payload");

        std::ranges::copy_n(payload.data(), dst.size(), dst.data());
        return payload.subspan(dst.size());
      } else {
        for (auto& elem : out) {
          payload = decode(elem, payload);
        }
        return payload;
      }
    } else if constexpr (Trivial<T>) {
      std::span tmp{&out, 1};
      return decode(tmp, payload);
    } else if constexpr (std::is_same_v<T, std::string>) {
      size_t size{};
      payload = decode(size, payload);
      out.resize(size);
      std::span tmp{out.data(), out.size()};
      return decode(tmp, payload);
    } else if constexpr (SpanConvertible<T>) {
      size_t size{};
      payload = decode(size, payload);
      out.resize(size);
      std::span tmp{out};
      return decode(tmp, payload);
    } else {
      template for (constexpr auto member :
        std::define_static_array(std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::current())))
      {
        payload = decode(out.[:member:], payload);
      }

      return payload;
    }
  }

  template <auto Name, typename Signature>
  struct RpcFunction;

  template <auto Name, typename R, typename... Args>
  struct RpcFunction<Name, R(Args...)> {
    using ReturnType = R;

    static consteval std::string_view signature()
    {
      return std::define_static_string(std::format("{} {}({})", id<R>(), Name, join<Args...>()));
    }

    static std::vector<std::byte> encodeCall(const Args&... args)
    {
      std::vector<std::byte> out;
      encode(out, signature());
      encode(out, 'C');
      (encode(out, args), ...);
      return out;
    }

    static std::span<const std::byte> verifySignature(std::span<const std::byte> payload)
    {
      std::string payloadSignature;
      payload = decode(payloadSignature, payload);
      C8_ASSERT(payloadSignature == signature(), "Signature mismatch expected: \"{}\", but got: \"{}\"", signature(),
        payloadSignature);
      return payload;
    }

    static std::span<const std::byte> verifyType(std::span<const std::byte> payload, char expectedType)
    {
      char type{};
      payload = decode(type, payload);
      C8_ASSERT(type == expectedType, "Type mismatch expected: \"{}\", but got: \"{}\"", expectedType, type);
      return payload;
    }

    template <typename F>
    static std::vector<std::byte> decodeCall(std::span<const std::byte> payload, F&& func)
    {
      payload = verifySignature(payload);
      payload = verifyType(payload, 'C');

      std::tuple<Args...> tuple;
      std::apply([&payload](auto&... args) mutable { ((payload = decode(args, payload)), ...); }, tuple);
      C8_ASSERT(payload.size() == 0, "Payload data was not fully consumed");

      std::vector<std::byte> out;

      encode(out, signature());

      try {
        if constexpr (std::is_same_v<R, void>) {
          std::apply(std::forward<F>(func), std::move(tuple));
          encode(out, 'R');
        } else {
          R ret = std::apply(std::forward<F>(func), std::move(tuple));
          encode(out, 'R');
          encode(out, ret);
        }
      } catch (const std::exception& e) {
        C8_TRACE("RpcEngine Error: {}", e.what());
        encode(out, 'E');
        encode(out, std::string_view(e.what()));
      }

      return out;
    }

    static std::expected<R, std::string> decodeReturn(std::span<const std::byte> payload)
    {
      payload = verifySignature(payload);

      char type{};
      payload = decode(type, payload);

      if (type == 'R') {
        if constexpr (!std::is_same_v<R, void>) {
          R out{};
          payload = decode(out, payload);
          C8_ASSERT(payload.size() == 0, "Payload data was not fully consumed");
          return out;
        } else {
          C8_ASSERT(payload.size() == 0, "Payload data was not fully consumed");
          return {};
        }
      } else if (type == 'E') {
        std::string error;
        payload = decode(error, payload);
        C8_ASSERT(payload.size() == 0, "Payload data was not fully consumed");
        return std::unexpected(error);
      } else {
        C8_ABORT("Type mismatch expected: \"{}\" or \"{}\", but got: \"{}\"", 'R', 'E', type);
      }
    }
  };

  struct RpcEngine {
  public:
    template <typename RpcFunctionType, typename F>
    void registerHandler(F&& handler)
    {
      C8_ASSERT(!map_.contains(RpcFunctionType::signature()), "Handler \"{}\" already registered.",
        RpcFunctionType::signature());

      map_[RpcFunctionType::signature()] = [handler = std::move(handler)](
                                             std::span<const std::byte> payload) -> std::vector<std::byte> {
        return RpcFunctionType::decodeCall(payload, handler);
      };
    }

    std::vector<std::byte> dispatchCall(std::span<const std::byte> payload);

  private:
    std::unordered_map<std::string_view, std::function<std::vector<std::byte>(std::span<const std::byte>)>> map_;
  };
} // namespace C8::Core
