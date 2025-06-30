#include "RawHandle.h"

namespace yawl {
RawWindowHandle::RawWindowHandle() : type(Type::None), handle(std::nullopt) {}
RawWindowHandle::RawWindowHandle(Type type, Handle handle)
    : type(type), handle(handle) {}
RawWindowHandle::RawWindowHandle(xcb_connection_t *connection,
                                 xcb_window_t window)
    : type(RawWindowHandle::Type::X11) {
  Handle h;
  h.x11.connection = connection;
  h.x11.window = window;
  handle = h;
}
} // namespace yawl