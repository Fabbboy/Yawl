#include "Event/XPoller.h"

#ifdef HAVE_X11
#include "Event/Loop.h"
#include <External/x11.h>
#include <cstdlib>

namespace yawl {

// Static member definition
xcb_atom_t XPoller::cached_wm_delete_window = XCB_ATOM_NONE;

void XPoller::poll(EventLoop &loop, WindowId id, Window &window) {
  RawWindowHandle handle = window.getWindowHandle();
  if (handle.getType() != RawWindowHandle::Type::X11)
    return;

  auto optHandle = handle.getHandle();
  if (!optHandle)
    return;

  const auto &h = optHandle->get();
  xcb_generic_event_t *ev = nullptr;
  while ((ev = xcb_poll_for_event(h.x11.connection)) != nullptr) {
    handleEvent(loop, id, ev);
    std::free(ev);
  }
}

void XPoller::handleEvent(EventLoop &loop, WindowId id, xcb_generic_event_t *ev) {
  uint8_t type = ev->response_type & ~0x80;
  switch (type) {
  case XCB_CLIENT_MESSAGE:
    handleClientMessage(loop, id,
                        reinterpret_cast<xcb_client_message_event_t *>(ev));
    break;
  case XCB_DESTROY_NOTIFY:
    handleDestroyNotify(loop, id,
                        reinterpret_cast<xcb_destroy_notify_event_t *>(ev));
    break;
  default:
    break;
  }
}

xcb_atom_t XPoller::getWmDeleteWindowAtom(xcb_connection_t *conn) {
  if (cached_wm_delete_window == XCB_ATOM_NONE) {
    xcb_intern_atom_cookie_t cookie = xcb_intern_atom(conn, 0, 16, "WM_DELETE_WINDOW");
    xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(conn, cookie, nullptr);
    if (reply) {
      cached_wm_delete_window = reply->atom;
      std::free(reply);
    }
  }
  return cached_wm_delete_window;
}

void XPoller::handleClientMessage(EventLoop &loop, WindowId id,
                                  xcb_client_message_event_t *ev) {
  // Get the connection to look up the WM_DELETE_WINDOW atom
  RawWindowHandle handle = loop.windows[id]->getWindowHandle();
  if (handle.getType() != RawWindowHandle::Type::X11)
    return;

  auto optHandle = handle.getHandle();
  if (!optHandle)
    return;

  const auto &h = optHandle->get();
  xcb_atom_t wm_delete_window = getWmDeleteWindowAtom(h.x11.connection);
  
  // Check if this is a WM_DELETE_WINDOW message
  if (ev->data.data32[0] == wm_delete_window) {
    Event e{Event::Type::CloseRequest, {}};
    loop.queueEvent(id, e);
  }
}

void XPoller::handleDestroyNotify(EventLoop &loop, WindowId id,
                                  xcb_destroy_notify_event_t *ev) {
  (void)ev;
  Event e{Event::Type::CloseRequest, {}};
  loop.queueEvent(id, e);
}

} // namespace yawl
#endif // HAVE_X11
