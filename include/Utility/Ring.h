#pragma once

#include "Result.h"
#include <array>
#include <cstddef>
#include <type_traits>

namespace yawl {

template <typename T, std::size_t Size> class RingBuffer {
public:
  enum class Error { Empty, Full };
  RingBuffer() : head(0), tail(0), full(false) {}

  Result<void, Error> push(const T &item) {
    if (full)
      return Err(Error::Full);

    new (&getBuffer()[head]) T(item);
    head = (head + 1) % Size;
    full = head == tail;
    return Ok();
  }

  Result<T, Error> pop() {
    if (isEmpty())
      return Err(Error::Empty);

    T item = std::move(getBuffer()[tail]);
    getBuffer()[tail].~T();
    full = false;
    tail = (tail + 1) % Size;
    return Ok(std::move(item));
  }

  bool isEmpty() const { return !full && (head == tail); }

  bool isFull() const { return full; }

  void reset() {
    head = tail = 0;
    full = false;
  }

  std::size_t size() const {
    if (full)
      return Size;
    if (head >= tail)
      return head - tail;
    return Size + head - tail;
  }

private:
  T *getBuffer() { return reinterpret_cast<T *>(buffer.data()); }

  const T *getBuffer() const {
    return reinterpret_cast<const T *>(buffer.data());
  }

  std::array<std::aligned_storage_t<sizeof(T), alignof(T)>, Size> buffer;
  std::size_t head;
  std::size_t tail;
  bool full;
};

} // namespace yawl
