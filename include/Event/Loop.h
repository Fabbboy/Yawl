#pragma once

#include "Event/Event.h"
#include "Event/Handler.h"
#include "Event/Poller.h"
#include "Utility/Ring.h"
#include "Windowing/Window.h"
#include <memory>
#include <vector>
#ifdef HAVE_X11
#include <unordered_map>
#include <optional>
#include <External/x11.h>
#endif

namespace yawl {
struct EventLoop {
private:
  struct QueuedEvent {
    WindowId id;
    Event event;
  };

  RingBuffer<QueuedEvent, 12> eventQueue;
  std::vector<std::unique_ptr<Window>> windows;
#ifdef HAVE_X11
  std::unordered_map<xcb_window_t, WindowId> x11WindowMap;
#endif
  bool running;
  Handler &handler;
  Poller *poller;

  void dispatchQueuedEvents();

public:
  EventLoop(Handler &h, Poller *poller = nullptr);

#ifdef HAVE_X11
  std::optional<WindowId> getWindowId(xcb_window_t win) const;
#endif

  WindowId mount(std::unique_ptr<Window> window);
  void unmount(WindowId id);

  void queueEvent(WindowId id, const Event &event);

  void run();

  inline void stop() { running = false; }
  inline void resume() { running = true; }
};
} // namespace yawl
