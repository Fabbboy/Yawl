#pragma once

namespace yawl {
struct EventLoop;

struct Poller {
  virtual ~Poller() = default;
  virtual void poll(EventLoop &loop) = 0;
};
} // namespace yawl
