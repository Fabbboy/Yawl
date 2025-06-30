#pragma once

#include <optional>
#include <type_traits>
namespace yawl {
template <typename T> struct NeverZero {
  static_assert(std::is_arithmetic_v<T>,
                "NeverZero can only be used with arithmetic types");

private:
  T value;

  constexpr explicit NeverZero(T value) = delete;

  struct Internal {};
  constexpr NeverZero(T value, Internal) : value(value) {}

public:
  NeverZero() : value(1) {}

  static constexpr std::optional<NeverZero> create(T value) {
    if (value <= static_cast<T>(0)) {
      return std::nullopt;
    }
    return NeverZero(value, Internal{});
  }

  constexpr T get() const { return value; }
  constexpr operator T() const { return value; }

  NeverZero(const NeverZero &other) : value(other.value) {}
  NeverZero &operator=(const NeverZero &other) {
    if (this != &other) {
      value = other.value;
    }
    return *this;
  }

  NeverZero(NeverZero &&other) noexcept : value(other.value) {
    other.value = 1;
  }

  NeverZero &operator=(NeverZero &&other) noexcept {
    if (this != &other) {
      value = other.value;
      other.value = 1;
    }
    return *this;
  }
};

template <typename T> struct Size {
  static_assert(std::is_arithmetic_v<T>,
                "Size can only be used with arithmetic types");

private:
  NeverZero<T> height;
  NeverZero<T> width;

  struct Internal {};

  constexpr Size(NeverZero<T> w, NeverZero<T> h, Internal)
      : width(w), height(h) {}

public:
  constexpr Size() : width(NeverZero<T>{}), height(NeverZero<T>{}) {}

  static constexpr std::optional<Size> create(T width, T height) {
    auto w = NeverZero<T>::create(width);
    auto h = NeverZero<T>::create(height);
    if (!w || !h) {
      return std::nullopt;
    }
    return Size(*w, *h, Internal{});
  }

  constexpr T getWidth() const { return width.get(); }
  constexpr T getHeight() const { return height.get(); }
};

} // namespace yawl