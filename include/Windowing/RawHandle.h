#pragma once

#ifdef HAVE_X11
#include "External/x11.h"
#endif
#include <functional>
#include <optional>

namespace yawl {
struct RawWindowHandle {
public:
  enum class Type {
    None,
#ifdef HAVE_X11
    X11,
#endif
  };

  union Handle {
#ifdef HAVE_X11
    struct {
      xcb_connection_t *connection;
      xcb_window_t window;
    } x11;
#else
    struct {
    } none;
#endif
  };

private:
  Type type;
  std::optional<Handle> handle;

public:
  RawWindowHandle();
  RawWindowHandle(Type type, Handle handle);
#ifdef HAVE_X11
  RawWindowHandle(xcb_connection_t *connection, xcb_window_t window);
#endif

  Type getType() const { return type; }
  std::optional<std::reference_wrapper<const Handle>> getHandle() const {
    return handle;
  }
};

struct HasWindowHandle {
  virtual RawWindowHandle getWindowHandle() const = 0;
};
} // namespace yawl
