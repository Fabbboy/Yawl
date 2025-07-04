#pragma once

#ifdef HAVE_X11

#include "Windowing/Window.h"
#include "Utility/Result.h"
#include "Windowing/Descriptor.h"
#include "Windowing/RawHandle.h"
#include "Windowing/XClient.h"
#include <External/x11.h>

namespace yawl {

struct XWindow : public Window {
public:
  enum class Error {
    FailedToConnect,
    FailedToGetScreen,
    FailedToCreateWindow,
    FailedToModifyWindow,
    FailedToShowWindow,
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

  XWindow(xcb_connection_t *conn, xcb_screen_t *screen, xcb_window_t win,
          bool owning);
  static Result<xcb_screen_t *, Error> getScreen(xcb_connection_t *conn,
                                                 int screen_num);

  static Result<xcb_window_t, Error> createWindow(xcb_connection_t *conn,
                                                  xcb_screen_t *screen,
                                                  const Descriptor &desc);

  Result<void, Error> modifyStringProperty(xcb_atom_t property, xcb_atom_t type,
                                           std::string_view value);

public:
  ~XWindow();

  XWindow(const XWindow &) = delete;
  XWindow &operator=(const XWindow &) = delete;

  XWindow(XWindow &&other) noexcept;
  XWindow &operator=(XWindow &&other) noexcept;

  RawWindowHandle getWindowHandle() const override {
    return RawWindowHandle(connection, window);
  }

  xcb_connection_t *getConnection() const { return connection; }
  xcb_window_t getWindow() const { return window; }
  bool isOwningWindow() const { return isOwning; }

  static Result<XWindow, Error> create(XClient &client, const Descriptor &desc);
  static Result<XWindow, Error> create(const Descriptor &desc);
};
} // namespace yawl

#endif // HAVE_X11
