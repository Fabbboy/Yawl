#pragma once

#include "Result.h"
#include <array>
#include <cstddef>

namespace yawl {

template <typename T, std::size_t Size> class RingBuffer {
public:
  enum class Error { Empty, Full };

  RingBuffer() : head(0), tail(0), full(false) {}

  Result<void, Error> push(const T &item) {
    if (full)
      return Err(Error::Full);

    buffer[head] = item;
    head = (head + 1) % Size;
    full = head == tail;
    return Ok();
  }

  Result<T, Error> pop() {
    if (isEmpty())
      return Err(Error::Empty);

    T item = std::move(buffer[tail]);
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
  std::array<T, Size> buffer;
  std::size_t head;
  std::size_t tail;
  bool full;
};

} // namespace yawl
