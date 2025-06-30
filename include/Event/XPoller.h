#pragma once

#ifdef HAVE_X11
#include "Event/Handler.h"
#include "Windowing/Window.h"
#include <External/x11.h>

namespace yawl {
struct EventLoop;

struct XPoller {
  void poll(EventLoop &loop, WindowId id, Window &window);

private:
  void handleEvent(EventLoop &loop, WindowId id, xcb_generic_event_t *ev);
  void handleClientMessage(EventLoop &loop, WindowId id,
                           xcb_client_message_event_t *ev);
  void handleDestroyNotify(EventLoop &loop, WindowId id,
                           xcb_destroy_notify_event_t *ev);
  
  // Cache for WM_DELETE_WINDOW atom lookup
  xcb_atom_t getWmDeleteWindowAtom(xcb_connection_t *conn);
  
  static xcb_atom_t cached_wm_delete_window;
};
} // namespace yawl
#endif // HAVE_X11
