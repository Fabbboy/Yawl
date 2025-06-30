#include "Windowing/XClient.h"
#ifdef HAVE_X11
#include "Event/XPoller.h"
#include <cstdlib>
#include <memory>

namespace yawl {

XClient::XClient() : connection(nullptr), screen(nullptr), poller(nullptr) {}

XClient::XClient(xcb_connection_t *conn, xcb_screen_t *scr, std::unique_ptr<XPoller> p)
    : connection(conn), screen(scr), poller(std::move(p)) {}

XClient::~XClient() {
  if (connection)
    xcb_disconnect(connection);
}

XClient::XClient(XClient &&other) noexcept
    : connection(other.connection), screen(other.screen), poller(std::move(other.poller)) {
  other.connection = nullptr;
  other.screen = nullptr;
}

XClient &XClient::operator=(XClient &&other) noexcept {
  if (this != &other) {
    if (connection)
      xcb_disconnect(connection);
    connection = other.connection;
    screen = other.screen;
    poller = std::move(other.poller);
    other.connection = nullptr;
    other.screen = nullptr;
  }
  return *this;
}

Result<XClient, XClient::Error> XClient::connect(std::optional<int> screenIndex) {
  int default_screen;
  xcb_connection_t *conn = xcb_connect(nullptr, &default_screen);
  if (xcb_connection_has_error(conn)) {
    return Err(Error::FailedToConnect);
  }

  int target = screenIndex.value_or(default_screen);
  const xcb_setup_t *setup = xcb_get_setup(conn);
  xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);
  for (int i = 0; i < target && iter.rem > 1; ++i)
    xcb_screen_next(&iter);

  if (iter.rem == 0 || iter.data == nullptr) {
    xcb_disconnect(conn);
    return Err(Error::FailedToGetScreen);
  }
  xcb_screen_t *scr = iter.data;

  auto poller = std::make_unique<XPoller>(conn);
  if (!poller)
    return Err(Error::FailedToInitAtoms);

  return Ok(XClient(conn, scr, std::move(poller)));
}

} // namespace yawl
#endif // HAVE_X11
