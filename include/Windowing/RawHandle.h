#pragma once

#ifdef HAVE_X11
#include "External/x11.h"
#endif
#include <functional>
#include <optional>

namespace yawl {
// feel free to reuse this in other places
enum class BackendType {
  None,
#ifdef HAVE_X11
  X11,
#endif
#ifdef HAVE_WAYLAND
  Wayland,
#endif
};

struct RawWindowHandle {
public:
  union Handle {
#ifdef HAVE_X11
    struct {
      xcb_connection_t *connection;
      xcb_window_t window;
    } x11;
#endif
#ifdef HAVE_WAYLAND
    #warning "Wayland support is not implemented yet"
#endif
  };

private:
  BackendType type;
  std::optional<Handle> handle;

public:
  RawWindowHandle();
  RawWindowHandle(BackendType type, Handle handle);
#ifdef HAVE_X11
  RawWindowHandle(xcb_connection_t *connection, xcb_window_t window);
#endif

  BackendType getType() const { return type; }
  std::optional<std::reference_wrapper<const Handle>> getHandle() const {
    return handle;
  }
};

struct HasWindowHandle {
  virtual RawWindowHandle getWindowHandle() const = 0;
};
} // namespace yawl
