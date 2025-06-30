#include "utility/Result.h"
#include "utility/Value.h"
#include <XWindow.h>
#include <cstdint>
#include <string_view>
#include <sys/types.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>

namespace yawl {
XWindow::XWindow(xcb_connection_t *conn, xcb_screen_t *screen, xcb_window_t win,
                 bool owning)
    : connection(conn), screen(screen), window(win), isOwning(owning) {}

XWindow::~XWindow() {
  if (isOwning && connection && window) {
    xcb_destroy_window(connection, window);
    xcb_disconnect(connection);
  }
}

XWindow::XWindow(XWindow &&other) noexcept
    : connection(other.connection), window(other.window),
      isOwning(other.isOwning) {
  other.connection = nullptr;
  other.screen = nullptr;
  other.window = 0;
  other.isOwning = false;
}

XWindow &XWindow::operator=(XWindow &&other) noexcept {
  if (this != &other) {
    if (isOwning) {
      xcb_destroy_window(connection, window);
    }
    connection = other.connection;
    screen = other.screen;
    window = other.window;
    isOwning = other.isOwning;

    other.connection = nullptr;
    other.screen = nullptr;
    other.window = 0;
    other.isOwning = false;
  }
  return *this;
}

Result<xcb_screen_t *, XWindow::Error>
XWindow::getScreen(xcb_connection_t *conn, int index) {
  const xcb_setup_t *setup = xcb_get_setup(conn);
  xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);

  for (int i = 0; i < index && iter.rem > 1; ++i) {
    xcb_screen_next(&iter);
  }

  if (iter.rem == 0 || iter.data == nullptr) {
    return Err(XWindow::Error::FailedToGetScreen);
  }

  return Ok(iter.data);
}

Result<xcb_window_t, XWindow::Error>
XWindow::createWindow(xcb_connection_t *conn, xcb_screen_t *screen,
                      const Descriptor &desc) {
  std::uint16_t value_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
  std::uint32_t value_list[2] = {
      screen->white_pixel,
      XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_STRUCTURE_NOTIFY |
          XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_POINTER_MOTION |
          XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE};

  xcb_window_t window = xcb_generate_id(conn);
  xcb_void_cookie_t status = xcb_create_window(
      conn, XCB_COPY_FROM_PARENT, window, screen->root, 0, 0,
      clampToU16(desc.dimensions.getWidth()),
      clampToU16(desc.dimensions.getHeight()), 0, XCB_WINDOW_CLASS_INPUT_OUTPUT,
      screen->root_visual, value_mask, value_list);

  xcb_generic_error_t *error = xcb_request_check(conn, status);
  if (error) {
    xcb_disconnect(conn);
    return Err(XWindow::Error::FailedToCreateWindow);
  }

  return Ok(window);
}

Result<void, XWindow::Error>
XWindow::modifyStringProperty(xcb_atom_t property, xcb_atom_t type,
                              std::string_view value) {
  if (value.empty()) {
    xcb_delete_property(connection, window, property);
    return Ok();
  }

  xcb_void_cookie_t status = xcb_change_property(
      connection, XCB_PROP_MODE_REPLACE, window, property, type,
      static_cast<uint8_t>(PropertyFormat::Char), value.size(), value.data());

  xcb_generic_error_t *error = xcb_request_check(connection, status);
  if (error) {
    xcb_disconnect(connection);
    return Err(XWindow::Error::FailedToModifyWindow);
  }

  return Ok();
}

Result<XWindow, XWindow::Error> XWindow::create(const Descriptor &desc) {
  int default_screen_index;
  xcb_connection_t *conn = xcb_connect(nullptr, &default_screen_index);
  if (xcb_connection_has_error(conn)) {
    return Err(Error::FailedToConnect);
  }

  int target_screen_index = desc.screen.value_or(default_screen_index);
  auto screen_result = getScreen(conn, target_screen_index);

  if (screen_result.is_err() && target_screen_index != default_screen_index) {
    screen_result = getScreen(conn, default_screen_index);
  }

  if (screen_result.is_err()) {
    xcb_disconnect(conn);
    return Err(screen_result.error());
  }

  xcb_screen_t *screen = screen_result.value();

  auto window_result = createWindow(conn, screen, desc);
  if (window_result.is_err()) {
    xcb_disconnect(conn);
    return Err(window_result.error());
  }

  xcb_window_t window = window_result.value();

  XWindow xwindow(conn, screen, window, true);
  auto modify_res = xwindow.modifyStringProperty(XCB_ATOM_WM_NAME,
                                                 XCB_ATOM_STRING, desc.name);

  if (modify_res.is_err()) {
    xcb_destroy_window(conn, window);
    xcb_disconnect(conn);
    return Err(modify_res.error());
  }

  xcb_void_cookie_t status = xcb_map_window(conn, window);
  xcb_generic_error_t *error = xcb_request_check(conn, status);
  if (error) {
    xcb_destroy_window(conn, window);
    xcb_disconnect(conn);
    return Err(XWindow::Error::FailedToShowWindow);
  }

  xcb_flush(conn);

  return Ok(std::move(xwindow));
}

} // namespace yawl