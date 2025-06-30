#include "Event/Loop.h"


namespace yawl {

EventLoop::EventLoop(Handler &h, Poller *p)
    : running(true), handler(h), poller(p) {}


WindowId EventLoop::mount(std::unique_ptr<Window> window) {
  windows.push_back(std::move(window));
  WindowId id = windows.size() - 1;
  if (poller)
    poller->registerWindow(id, *windows.back());
  return id;
}

void EventLoop::unmount(WindowId id) {
  if (id < windows.size()) {
    if (poller)
      poller->unregisterWindow(id, *windows[id]);
    windows.erase(windows.begin() + id);
  }
}

void EventLoop::queueEvent(WindowId id, const Event &event) {
  eventQueue.push({id, event});
}

void EventLoop::dispatchQueuedEvents() {
  while (!eventQueue.isEmpty()) {
    auto res = eventQueue.pop();
    if (res.is_ok()) {
      auto qe = std::move(res.value());
      handler.onEvent(*this, qe.id, qe.event);
    }
  }
}

void EventLoop::run() {
  while (!windows.empty() && running) {
    if (poller)
      poller->poll(*this);
    dispatchQueuedEvents();
  }
}

} // namespace yawl
