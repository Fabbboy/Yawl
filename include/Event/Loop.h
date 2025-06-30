#pragma once

#include "Event/Event.h"
#include "Event/Handler.h"
#include "Event/Poller.h"
#include "Utility/Ring.h"
#include "Windowing/Window.h"
#include <memory>
#include <vector>


namespace yawl {
struct EventLoop {
private:
  struct QueuedEvent {
    WindowId id;
    Event event;
  };

  RingBuffer<QueuedEvent, 12> eventQueue;
  std::vector<std::unique_ptr<Window>> windows;
  bool running;
  Handler &handler;
  Poller *poller;

  void dispatchQueuedEvents();

public:
  EventLoop(Handler &h, Poller *poller = nullptr);


  WindowId mount(std::unique_ptr<Window> window);
  void unmount(WindowId id);

  void queueEvent(WindowId id, const Event &event);

  void run();

  inline void stop() { running = false; }
  inline void resume() { running = true; }
};
} // namespace yawl
