#include "Event/Loop.h"
#ifdef HAVE_X11
#include "Event/XPoller.h"
#endif

namespace yawl {

EventLoop::EventLoop(Handler &h, Poller *p)
    : running(true), handler(h), poller(p) {}

#ifdef HAVE_X11
std::optional<WindowId> EventLoop::getWindowId(xcb_window_t win) const {
  auto it = x11WindowMap.find(win);
  if (it == x11WindowMap.end())
    return std::nullopt;
  return it->second;
}
#endif

WindowId EventLoop::mount(std::unique_ptr<Window> window) {
  windows.push_back(std::move(window));
  WindowId id = windows.size() - 1;
#ifdef HAVE_X11
  RawWindowHandle handle = windows.back()->getWindowHandle();
  if (handle.getType() == BackendType::X11) {
    auto opt = handle.getHandle();
    if (opt)
      x11WindowMap[opt->get().x11.window] = id;
  }
#endif
  return id;
}

void EventLoop::unmount(WindowId id) {
  if (id < windows.size()) {
#ifdef HAVE_X11
    RawWindowHandle handle = windows[id]->getWindowHandle();
    if (handle.getType() == BackendType::X11) {
      auto opt = handle.getHandle();
      if (opt)
        x11WindowMap.erase(opt->get().x11.window);
    }
#endif
    windows.erase(windows.begin() + id);
#ifdef HAVE_X11
    // rebuild mapping to update ids
    x11WindowMap.clear();
    for (WindowId i = 0; i < windows.size(); ++i) {
      RawWindowHandle h = windows[i]->getWindowHandle();
      if (h.getType() == BackendType::X11) {
        auto opt = h.getHandle();
        if (opt)
          x11WindowMap[opt->get().x11.window] = i;
      }
    }
#endif
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
