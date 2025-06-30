#pragma once

#include "Descriptor.h"
#include "RawHandle.h"
#include "utility/Result.h"
#include <external/x11.h>
#include <xcb/xproto.h>

namespace yawl {

struct XWindow : public HasWindowHandle {
public:
  enum class Error {
    FailedToConnect,
    FailedToGetScreen,
    FailedToCreateWindow,
    FailedToModifyWindow,
  };

  enum class PropertyFormat : uint8_t {
    Char = 8,
    Short = 16,
    Int = 32,
  };

private:
  xcb_connection_t *connection;
  xcb_screen_t *screen;
  xcb_window_t window;
  bool isOwning;

private:
  XWindow(xcb_connection_t *conn, xcb_screen_t *screen, xcb_window_t win,
          bool owning);
  static Result<xcb_screen_t *, Error> getScreen(xcb_connection_t *conn,
                                                 int screen_num);

  static Result<xcb_window_t, Error> createWindow(xcb_connection_t *conn,
                                                  xcb_screen_t *screen,
                                                  const Descriptor &desc);

  Result<void, Error> modifyStringProperty(xcb_atom_t property, xcb_atom_t type,
                                           PropertyFormat format,
                                           std::string_view value);

public:
  ~XWindow();

  XWindow(const XWindow &) = delete;
  XWindow &operator=(const XWindow &) = delete;

  XWindow(XWindow &&other) noexcept;
  XWindow &operator=(XWindow &&other) noexcept;

public:
  RawWindowHandle getWindowHandle() const override {
    return RawWindowHandle(connection, window);
  };

public:
  xcb_connection_t *getConnection() const { return connection; }
  xcb_window_t getWindow() const { return window; }
  bool isOwningWindow() const { return isOwning; }

  static Result<XWindow, Error> create(const Descriptor &desc);
};
} // namespace yawl
