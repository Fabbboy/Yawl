#pragma once

#include "Event/Event.h"
#include "Utility/Ring.h"
namespace yawl {
struct EventLoop {
private:
  RingBuffer<Event, 12> eventQueue;

public:
  EventLoop() = default;
};
} // namespace yawl