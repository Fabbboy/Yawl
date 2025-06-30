#pragma once

#include "Event/Event.h"
#include "Event/Handler.h"
#include "Utility/Ring.h"
#include "Windowing/Window.h"
#include <memory>
#include <vector>

namespace yawl {
#ifdef HAVE_X11
struct XPoller;
#endif
struct EventLoop {
#ifdef HAVE_X11
  friend struct XPoller;
#endif
private:
  struct QueuedEvent {
    WindowId id;
    Event event;
  };

  RingBuffer<QueuedEvent, 12> eventQueue;
  std::vector<std::unique_ptr<Window>> windows;
  bool running;
  Handler &handler;

  void dispatchQueuedEvents();
  void queueEvent(WindowId id, const Event &event);

public:
  EventLoop(Handler &h);

  WindowId mount(std::unique_ptr<Window> window);
  void unmount(WindowId id);

  void run();

  inline void stop() { running = false; }
  inline void resume() { running = true; }
};
} // namespace yawl
