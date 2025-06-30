#pragma once

#include "Event/Event.h"
#include "Event/Handler.h"
#include "Utility/Ring.h"
#include "Windowing/Window.h"
#include <memory>
#include <vector>

namespace yawl {
struct EventLoop {
private:
  RingBuffer<Event, 12> eventQueue;
  std::vector<std::unique_ptr<Window>> windows;
  bool running;
  Handler &handler;

public:
  EventLoop(Handler &h);

  WindowId mount(std::unique_ptr<Window> window);
  void unmount(WindowId id);

  void run();

  inline void stop() { running = false; }
  inline void resume() { running = true; }
};
} // namespace yawl