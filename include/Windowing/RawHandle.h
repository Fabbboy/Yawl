#pragma once

#include "External/x11.h"
#include <functional>
#include <optional>

namespace yawl {
struct RawWindowHandle {
public:
  enum class Type {
    None,
    X11,
  };

  union Handle {
    struct {
      xcb_connection_t *connection;
      xcb_window_t window;
    } x11;
  };

private:
  Type type;
  std::optional<Handle> handle;

public:
  RawWindowHandle();
  RawWindowHandle(Type type, Handle handle);
  RawWindowHandle(xcb_connection_t *connection, xcb_window_t window);

  Type getType() const { return type; }
  std::optional<std::reference_wrapper<const Handle>> getHandle() const {
    return handle;
  }
};

struct HasWindowHandle {
  virtual RawWindowHandle getWindowHandle() const = 0;
};
} // namespace yawl