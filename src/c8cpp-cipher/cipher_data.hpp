#pragma once

#include <algorithm>
#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace C8::Cipher {
  struct DV_RW {
    using value_type = std::byte;

    template <typename T, typename Self>
    using rtype = std::conditional_t<std::is_const_v<Self>, std::add_const_t<T>, T>;

    static std::span<value_type> construct(std::string& value)
    {
      return std::as_writable_bytes(std::span{value.data(), value.size()});
    }

    static std::span<value_type> construct(std::span<std::byte> value)
    {
      return value;
    }
  };

  struct DV_RO {
    using value_type = const std::byte;

    template <typename T, typename Self>
    using rtype = std::add_const_t<T>;

    static std::span<value_type> construct(std::string_view value)
    {
      return std::as_bytes(std::span{value});
    }

    static std::span<value_type> construct(std::span<const std::byte> value)
    {
      return value;
    }
  };

  template <typename DVType>
  struct DV {
    template <typename T>
    DV(T&& value) : value_(DVType::construct(std::forward<T>(value)))
    {
    }

    DV(const DV<DVType>& other) : value_(other.value_) {}
    DV(DV<DVType>& other) : value_(other.value_) {}
    DV(DV<DVType>&& other) : value_(other.value_) {}

    template <typename T, typename Self>
    using rtype = typename DVType::rtype<T, Self>;

    std::string string() const
    {
      return std::string(string_view());
    }

    std::string_view string_view() const
    {
      return std::string_view(reinterpret_cast<const char*>(value_.data()), value_.size());
    }

    template <typename Self>
    std::span<rtype<std::byte, Self>> span(this Self&& self)
    {
      return self.value_;
    }

    template <typename Self>
    rtype<uint8_t, Self>* asU8(this Self&& self)
    {
      return reinterpret_cast<rtype<uint8_t, Self>*>(self.value_.data());
    }

    template <typename Self>
    rtype<char, Self>* asChar(this Self&& self)
    {
      return reinterpret_cast<rtype<char, Self>*>(self.value_.data());
    }

    template <typename Self>
    rtype<std::byte, Self>* asByte(this Self&& self)
    {
      return reinterpret_cast<rtype<std::byte, Self>*>(self.value_.data());
    }

    DV<DVType> subview(size_t offset, size_t count = std::dynamic_extent)
    {
      return {value_.subspan(offset, count)};
    }

    template <typename Self>
    auto data(this Self&& self)
    {
      return self.asByte();
    }

    size_t size() const
    {
      return value_.size();
    }

    template <typename Policy>
    bool operator==(const DV<Policy>& other) const
    {
      const auto span = other.span();
      return std::equal(value_.begin(), value_.end(), span.begin(), span.end());
    }

  private:
    std::span<typename DVType::value_type> value_;
  };

  using CipherDataRO = DV<DV_RO>;
  using CipherDataRW = DV<DV_RW>;

  struct CipherData {
    CipherData() = default;
    CipherData(size_t size) : value_(size) {}
    CipherData(std::vector<std::byte>&& value) : value_(std::move(value)) {}
    CipherData(const CipherData&) = delete;
    CipherData(CipherData&&) = default;

    CipherData& operator=(const CipherData&) = delete;
    CipherData& operator=(CipherData&&) = default;

    CipherDataRO cview() const
    {
      return {value_};
    }

    CipherDataRO view() const
    {
      return {value_};
    }

    CipherDataRW view()
    {
      return {value_};
    }

    std::string string() const
    {
      return view().string();
    }

    std::string_view string_view() const
    {
      return view().string_view();
    }

    void resize(size_t size)
    {
      value_.resize(size);
    }

    void append(CipherDataRO view)
    {
      std::ranges::copy(view.span(), std::back_inserter(value_));
    }

    void purge()
    {
      std::ranges::fill(value_, std::byte{});
      value_ = std::vector<std::byte>{};
    }

    operator CipherDataRO() const
    {
      return {value_};
    }

    operator CipherDataRO()
    {
      return {value_};
    }

  private:
    std::vector<std::byte> value_;
  };
} // namespace C8::Cipher
