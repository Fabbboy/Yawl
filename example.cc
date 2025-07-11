#include <Yawl.h>
#include <iostream>

struct Handler : public yawl::Handler {
  void onEvent(yawl::EventLoop &loop, yawl::WindowId id, yawl::Event &event) override {
    switch (event.type) {
    case yawl::Event::Type::CloseRequest:
      std::cout << "Close request received for window ID: " << id << std::endl;
      loop.unmount(id);
      break;
    }
  }
};

int main() {
  Handler handler;
#ifdef HAVE_X11
  auto clientRes = yawl::XClient::connect();
  if (clientRes.is_err()) {
    std::cerr << "Failed to connect to X server" << std::endl;
    return 1;
  }
  yawl::XClient client = std::move(clientRes.value());
  yawl::EventLoop eventLoop(handler, client.getPoller());

  yawl::Descriptor desc;
  desc.setDimensions(yawl::Size<size_t>::create(800, 600).value())
      .setName("Yawl Example Window");
  auto result = yawl::XWindow::create(client, desc);
  if (result.is_err()) {
    std::cerr << "Failed to create window: " << static_cast<int>(result.error()) << std::endl;
    return 1;
  }

  yawl::XWindow window = std::move(result.value());
  std::cout << "Window created successfully!" << std::endl;

  eventLoop.mount(std::make_unique<yawl::XWindow>(std::move(window)));
#else
  yawl::EventLoop eventLoop(handler);
  std::cerr << "X11 support is not enabled. Please compile with X11 support." << std::endl;
  return 1;
#endif

  eventLoop.run();

  std::cout << "Event loop has ended." << std::endl;
  return 0;
}
