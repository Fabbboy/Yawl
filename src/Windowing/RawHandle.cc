#include "Windowing/RawHandle.h"

namespace yawl {
RawWindowHandle::RawWindowHandle() : type(BackendType::None), handle(std::nullopt) {}
RawWindowHandle::RawWindowHandle(BackendType type, Handle handle)
    : type(type), handle(handle) {}
#ifdef HAVE_X11
RawWindowHandle::RawWindowHandle(xcb_connection_t *connection,
                                 xcb_window_t window)
    : type(BackendType::X11) {
  Handle h;
  h.x11.connection = connection;
  h.x11.window = window;
  handle = h;
}
#endif
} // namespace yawl
