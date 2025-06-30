#include "Event/Loop.h"

namespace yawl {
EventLoop::EventLoop(Handler &h) : handler(h), running(true) {}

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

void EventLoop::run() {
  while (!windows.empty() && running) {
  }
}
} // namespace yawl