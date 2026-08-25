#pragma once

#include <string>
#include <variant>
#include <vector>

namespace C8::Db::Statement {
  template <typename T>
  concept ParamString =
    requires(T t) { requires std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>; };

  class Params {
  public:
    using value_type = std::variant<std::string, int32_t, int64_t, double>;

    template <typename T>
    Params& operator()(T value) {
      if constexpr (ParamString<T>) {
        params_.emplace_back(std::in_place_type<std::string>, std::move(value));
      } else {
        params_.emplace_back(std::move(value));
      }
      return *this;
    }

    Params operator+(const Params& other) const {
      Params result;
      result.params_.insert(result.params_.end(), params_.begin(), params_.end());
      result.params_.insert(result.params_.end(), other.params_.begin(), other.params_.end());
      return result;
    }

    const value_type& operator[](size_t index) const {
      return params_[index];
    }

    size_t size() const {
      return params_.size();
    }

    bool empty() const {
      return params_.empty();
    }

    auto begin() const {
      return params_.begin();
    }

    auto end() const {
      return params_.end();
    }

  private:
    std::vector<value_type> params_;
  };
} // namespace C8::Db::Statement
