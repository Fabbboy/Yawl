#pragma once

#include "Utility/Panic.h"
#include <optional>

namespace yawl {
template <typename T, typename E> class Result;

template <typename T> struct OkType {
  using Type = T;
  T value;

  OkType(const T &v) : value(v) {}
  OkType(T &&v) : value(std::move(v)) {}
};

template <typename E> struct ErrType {
  using Type = E;
  E error;

  ErrType(const E &e) : error(e) {}
  ErrType(E &&e) : error(std::move(e)) {}
};

template <typename T> OkType<std::decay_t<T>> Ok(T &&value) {
  using U = std::decay_t<T>;
  static_assert(!std::is_reference_v<U>,
                "use std::reference_wrapper to pass references to Ok");
  return OkType<U>{std::forward<T>(value)};
}

template <> struct OkType<void> {};
inline OkType<void> Ok() { return {}; }

template <typename E> ErrType<std::decay_t<E>> Err(E &&error) {
  using U = std::decay_t<E>;
  static_assert(!std::is_reference_v<U>,
                "use std::reference_wrapper to pass references to Err");
  return ErrType<U>{std::forward<E>(error)};
}

template <> struct ErrType<void> {};
inline ErrType<void> Err() { return {}; }

template <typename T, typename E> class Result {
  bool is_ok_;
  union {
    T value_;
    E error_;
  };

public:
  Result(OkType<T> ok) : is_ok_(true), value_(std::move(ok.value)) {}
  Result(ErrType<E> err) : is_ok_(false), error_(std::move(err.error)) {}

  Result(const Result &) = delete;
  Result &operator=(const Result &) = delete;

  Result(Result &&other) noexcept(std::is_nothrow_move_constructible_v<T> &&
                                  std::is_nothrow_move_constructible_v<E>) {
    is_ok_ = other.is_ok_;
    if (is_ok_) {
      new (&value_) T(std::move(other.value_));
    } else {
      new (&error_) E(std::move(other.error_));
    }
  }

  Result &operator=(Result &&other) noexcept {
    if (this != &other) {
      this->~Result();
      new (this) Result(std::move(other));
    }
    return *this;
  }

  ~Result() {
    if (is_ok_)
      value_.~T();
    else
      error_.~E();
  }

  bool is_ok() const { return is_ok_; }
  bool is_err() const { return !is_ok_; }

  std::optional<T> unwrap() {
    if (is_ok_)
      return std::move(value_);
    return std::nullopt;
  }

  std::optional<E> unwrap_err() {
    if (!is_ok_)
      return std::move(error_);
    return std::nullopt;
  }

  T &value() {
    if (!is_ok_)
      PANIC("Tried to access value() on Err");
    return value_;
  }

  E &error() {
    if (is_ok_)
      PANIC("Tried to access error() on Ok");
    return error_;
  }
};

template <typename E> class Result<void, E> {
  bool is_ok_;
  union {
    E error_;
  };

public:
  Result(OkType<void>) : is_ok_(true) {}
  Result(ErrType<E> err) : is_ok_(false), error_(std::move(err.error)) {}

  Result(const Result &) = delete;
  Result &operator=(const Result &) = delete;

  Result(Result &&other) noexcept(std::is_nothrow_move_constructible_v<E>) {
    is_ok_ = other.is_ok_;
    if (!is_ok_)
      new (&error_) E(std::move(other.error_));
  }

  Result &operator=(Result &&other) noexcept {
    if (this != &other) {
      this->~Result();
      new (this) Result(std::move(other));
    }
    return *this;
  }

  ~Result() {
    if (!is_ok_)
      error_.~E();
  }

  bool is_ok() const { return is_ok_; }
  bool is_err() const { return !is_ok_; }

  void unwrap() {
    if (!is_ok_)
      PANIC("Tried to unwrap Err in Result<void, E>");
  }

  std::optional<E> unwrap_err() {
    if (!is_ok_)
      return std::move(error_);
    return std::nullopt;
  }

  E &error() {
    if (is_ok_)
      PANIC("Tried to access error() on Ok");
    return error_;
  }
};

} // namespace yawl