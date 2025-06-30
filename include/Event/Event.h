#pragma once

namespace yawl {
struct Event {
public:
  enum class Type {
    CloseRequest,
  };

  union Data {};

public:
  Type type;
  Data data;

  Event(Type t, Data d) : type(t), data(d) {}

  Event(const Event &) = default;
  Event &operator=(const Event &) = default;

  Event(Event &&) noexcept = default;
  Event &operator=(Event &&) noexcept = default;

  ~Event() = default;
};
} // namespace yawl