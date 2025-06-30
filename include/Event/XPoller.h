#pragma once

#ifdef HAVE_X11
#include "Event/Poller.h"
#include <External/x11.h>
#include <unordered_map>

namespace yawl {
struct XPoller : public Poller {
  explicit XPoller(xcb_connection_t *conn);

  void poll(EventLoop &loop) override;

  void registerWindow(WindowId id, Window &window) override;
  void unregisterWindow(WindowId id, Window &window) override;

  xcb_atom_t wm_protocols;
  xcb_atom_t wm_delete_window;

private:
  void handleEvent(EventLoop &loop, xcb_generic_event_t *ev);
  void handleClientMessage(EventLoop &loop, xcb_client_message_event_t *ev);
  void handleDestroyNotify(EventLoop &loop, xcb_destroy_notify_event_t *ev);

  xcb_connection_t *connection;
  std::unordered_map<xcb_window_t, WindowId> windowMap;
};
} // namespace yawl
#endif // HAVE_X11
