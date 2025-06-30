#include "Event/Loop.h"
#ifdef HAVE_X11
#include "Event/XPoller.h"
#endif

namespace yawl {
EventLoop::EventLoop(Handler &h) : running(true), handler(h) {}

void EventLoop::queueEvent(WindowId id, const Event &event) {
  eventQueue.push({id, event});
}

WindowId EventLoop::mount(std::unique_ptr<Window> window) {
  windows.push_back(std::move(window));
  WindowId id = windows.size() - 1;
  return id;
}

void EventLoop::unmount(WindowId id) {
  if (id < windows.size()) {
    windows.erase(windows.begin() + id);
  }
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
    for (WindowId i = 0; i < windows.size(); ++i) {
#ifdef HAVE_X11
      XPoller poller;
      poller.poll(*this, i, *windows[i]);
#endif
    }

    dispatchQueuedEvents();
  }
}
} // namespace yawl
