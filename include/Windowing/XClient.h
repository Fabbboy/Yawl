#pragma once

#ifdef HAVE_X11
#include "Event/Poller.h"
#include "Utility/Result.h"
#include <External/x11.h>
#include <optional>
#include <memory>
#include "Event/XPoller.h"

namespace yawl {
struct EventLoop;

struct XClient {
  enum class Error {
    FailedToConnect,
    FailedToGetScreen,
    FailedToInitAtoms,
  };

private:
  xcb_connection_t *connection;
  xcb_screen_t *screen;
  std::unique_ptr<XPoller> poller;

  XClient(xcb_connection_t *conn, xcb_screen_t *scr, std::unique_ptr<XPoller> p);

public:
  XClient();
  ~XClient();
  XClient(XClient &&other) noexcept;
  XClient &operator=(XClient &&other) noexcept;

  static Result<XClient, Error> connect(std::optional<int> screenIndex = std::nullopt);

  xcb_connection_t *getConnection() const { return connection; }
  xcb_screen_t *getScreen() const { return screen; }
  XPoller *getPoller() const { return poller.get(); }
};

} // namespace yawl

#endif // HAVE_X11
