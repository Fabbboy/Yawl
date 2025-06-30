#include "Event/XPoller.h"
#ifdef HAVE_X11
#include "Event/Loop.h"
#include <cstdlib>

namespace yawl {

XPoller::XPoller(xcb_connection_t *conn)
    : connection(conn), wm_protocols(XCB_ATOM_NONE), wm_delete_window(XCB_ATOM_NONE) {
  xcb_intern_atom_cookie_t proto_cookie = xcb_intern_atom(conn, 1, 12, "WM_PROTOCOLS");
  xcb_intern_atom_cookie_t del_cookie = xcb_intern_atom(conn, 0, 16, "WM_DELETE_WINDOW");
  xcb_intern_atom_reply_t *proto_reply = xcb_intern_atom_reply(conn, proto_cookie, nullptr);
  xcb_intern_atom_reply_t *del_reply = xcb_intern_atom_reply(conn, del_cookie, nullptr);
  if (proto_reply && del_reply) {
    wm_protocols = proto_reply->atom;
    wm_delete_window = del_reply->atom;
  }
  if (proto_reply)
    std::free(proto_reply);
  if (del_reply)
    std::free(del_reply);
}

void XPoller::poll(EventLoop &loop) {
  xcb_generic_event_t *ev = nullptr;
  while ((ev = xcb_poll_for_event(connection)) != nullptr) {
    handleEvent(loop, ev);
    std::free(ev);
  }
}

void XPoller::handleEvent(EventLoop &loop, xcb_generic_event_t *ev) {
  uint8_t type = ev->response_type & ~0x80;
  switch (type) {
  case XCB_CLIENT_MESSAGE:
    handleClientMessage(loop, reinterpret_cast<xcb_client_message_event_t *>(ev));
    break;
  case XCB_DESTROY_NOTIFY:
    handleDestroyNotify(loop, reinterpret_cast<xcb_destroy_notify_event_t *>(ev));
    break;
  default:
    break;
  }
}

void XPoller::handleClientMessage(EventLoop &loop, xcb_client_message_event_t *ev) {
  auto optId = loop.getWindowId(ev->window);
  if (!optId)
    return;
  if (ev->data.data32[0] == wm_delete_window) {
    Event e{Event::Type::CloseRequest, {}};
    loop.queueEvent(*optId, e);
  }
}

void XPoller::handleDestroyNotify(EventLoop &loop, xcb_destroy_notify_event_t *ev) {
  auto optId = loop.getWindowId(ev->window);
  if (!optId)
    return;
  Event e{Event::Type::CloseRequest, {}};
  loop.queueEvent(*optId, e);
}

} // namespace yawl
#endif // HAVE_X11
