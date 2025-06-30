#pragma once

#include "Event/Event.h"
#include <cstddef>

namespace yawl {
struct EventLoop;
using WindowId = std::size_t;

struct Handler {
public:
  virtual ~Handler() = default;

  virtual void onEvent(EventLoop &loop, WindowId id, Event &event) = 0;
};
} // namespace yawl