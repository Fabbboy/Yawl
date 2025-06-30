#ifdef HAVE_X11

#include "Windowing/XWindow.h"
#include "Windowing/XClient.h"
#include "Event/XPoller.h"
#include "Utility/Result.h"
#include "Utility/Value.h"
#include <cstdint>
#include <cstdlib>
#include <string_view>
#include <xcb/xcb.h>
#include <xcb/xproto.h>

namespace yawl {

XWindow::XWindow(xcb_connection_t *conn, xcb_screen_t *scr, xcb_window_t win,
                 bool owning)
    : connection(conn), screen(scr), window(win), isOwning(owning) {}

XWindow::~XWindow() {
  if (connection && window) {
    xcb_destroy_window(connection, window);
    if (isOwning)
      xcb_disconnect(connection);
  }
}

XWindow::XWindow(XWindow &&other) noexcept
    : connection(other.connection), screen(other.screen), window(other.window),
      isOwning(other.isOwning) {
  other.connection = nullptr;
  other.screen = nullptr;
  other.window = 0;
  other.isOwning = false;
}

XWindow &XWindow::operator=(XWindow &&other) noexcept {
  if (this != &other) {
    if (connection && window) {
      xcb_destroy_window(connection, window);
      if (isOwning)
        xcb_disconnect(connection);
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
XWindow::createWindow(xcb_connection_t *conn, xcb_screen_t *scr,
                      const Descriptor &desc) {
  std::uint16_t mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
  std::uint32_t values[2] = {scr->white_pixel,
                             XCB_EVENT_MASK_EXPOSURE |
                                 XCB_EVENT_MASK_STRUCTURE_NOTIFY |
                                 XCB_EVENT_MASK_KEY_PRESS |
                                 XCB_EVENT_MASK_POINTER_MOTION |
                                 XCB_EVENT_MASK_BUTTON_PRESS |
                                 XCB_EVENT_MASK_BUTTON_RELEASE};

  xcb_window_t win = xcb_generate_id(conn);
  xcb_void_cookie_t status = xcb_create_window(
      conn, XCB_COPY_FROM_PARENT, win, scr->root, 0, 0,
      clampToU16(desc.dimensions.getWidth()),
      clampToU16(desc.dimensions.getHeight()), 0, XCB_WINDOW_CLASS_INPUT_OUTPUT,
      scr->root_visual, mask, values);

  xcb_generic_error_t *error = xcb_request_check(conn, status);
  if (error) {
    return Err(XWindow::Error::FailedToCreateWindow);
  }

  return Ok(win);
}

Result<void, XWindow::Error> XWindow::modifyStringProperty(xcb_atom_t property,
                                                           xcb_atom_t type,
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
    return Err(XWindow::Error::FailedToModifyWindow);
  }

  return Ok();
}

Result<XWindow, XWindow::Error> XWindow::create(XClient &client,
                                                const Descriptor &desc) {
  xcb_connection_t *conn = client.getConnection();
  xcb_screen_t *scr = client.getScreen();

  auto window_res = createWindow(conn, scr, desc);
  if (window_res.is_err()) {
    return Err(window_res.error());
  }

  xcb_window_t win = window_res.value();
  XWindow xw(conn, scr, win, false);

  XPoller *poller = client.getPoller();
  if (poller) {
    xcb_change_property(conn, XCB_PROP_MODE_REPLACE, win, poller->wm_protocols,
                        XCB_ATOM_ATOM, 32, 1, &poller->wm_delete_window);
  }

  auto mod_res = xw.modifyStringProperty(XCB_ATOM_WM_NAME, XCB_ATOM_STRING,
                                         desc.name);
  if (mod_res.is_err()) {
    xcb_destroy_window(conn, win);
    return Err(mod_res.error());
  }

  xcb_void_cookie_t status = xcb_map_window(conn, win);
  xcb_generic_error_t *error = xcb_request_check(conn, status);
  if (error) {
    xcb_destroy_window(conn, win);
    return Err(XWindow::Error::FailedToShowWindow);
  }

  xcb_flush(conn);

  return Ok(std::move(xw));
}

Result<XWindow, XWindow::Error> XWindow::create(const Descriptor &desc) {
  int default_screen;
  xcb_connection_t *conn = xcb_connect(nullptr, &default_screen);
  if (xcb_connection_has_error(conn)) {
    return Err(Error::FailedToConnect);
  }

  int target_screen = desc.screen.value_or(default_screen);
  auto screen_res = getScreen(conn, target_screen);
  if (screen_res.is_err() && target_screen != default_screen) {
    screen_res = getScreen(conn, default_screen);
  }
  if (screen_res.is_err()) {
    xcb_disconnect(conn);
    return Err(screen_res.error());
  }

  xcb_screen_t *scr = screen_res.value();
  auto window_res = createWindow(conn, scr, desc);
  if (window_res.is_err()) {
    xcb_disconnect(conn);
    return Err(window_res.error());
  }

  xcb_window_t win = window_res.value();

  // Set up WM_DELETE_WINDOW protocol for this window
  xcb_intern_atom_cookie_t proto_cookie = xcb_intern_atom(conn, 1, 12, "WM_PROTOCOLS");
  xcb_intern_atom_cookie_t del_cookie = xcb_intern_atom(conn, 0, 16, "WM_DELETE_WINDOW");
  xcb_intern_atom_reply_t *proto_reply = xcb_intern_atom_reply(conn, proto_cookie, nullptr);
  xcb_intern_atom_reply_t *del_reply = xcb_intern_atom_reply(conn, del_cookie, nullptr);
  if (proto_reply && del_reply) {
    xcb_change_property(conn, XCB_PROP_MODE_REPLACE, win, proto_reply->atom,
                        XCB_ATOM_ATOM, 32, 1, &del_reply->atom);
  }
  if (proto_reply)
    std::free(proto_reply);
  if (del_reply)
    std::free(del_reply);

  XWindow xw(conn, scr, win, true);
  auto mod_res = xw.modifyStringProperty(XCB_ATOM_WM_NAME, XCB_ATOM_STRING,
                                         desc.name);
  if (mod_res.is_err()) {
    xcb_destroy_window(conn, win);
    xcb_disconnect(conn);
    return Err(mod_res.error());
  }

  xcb_void_cookie_t status = xcb_map_window(conn, win);
  xcb_generic_error_t *error = xcb_request_check(conn, status);
  if (error) {
    xcb_destroy_window(conn, win);
    xcb_disconnect(conn);
    return Err(XWindow::Error::FailedToShowWindow);
  }

  xcb_flush(conn);

  return Ok(std::move(xw));
}

} // namespace yawl

#endif // HAVE_X11
