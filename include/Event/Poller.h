#pragma once

#include "Event/Handler.h" // for WindowId
#include "Windowing/Window.h"

namespace yawl {
struct EventLoop;

struct Poller {
  virtual ~Poller() = default;
  virtual void poll(EventLoop &loop) = 0;
  virtual void registerWindow(WindowId id, Window &window) {}
  virtual void unregisterWindow(WindowId id, Window &window) {}
};
} // namespace yawl
