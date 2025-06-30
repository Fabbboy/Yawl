
#include <Yawl.h>
#include <iostream>

struct Handler : public yawl::Handler {
  void onEvent(yawl::EventLoop &loop, yawl::WindowId id,
               yawl::Event &event) override {
    if (event.type == yawl::Event::Type::CloseRequest) {
      std::cout << "Received close request for window ID: " << id << std::endl;
      loop.unmount(id);
    }
  }
};

int main() {
  Handler handler;
  yawl::EventLoop eventLoop(handler);

#ifdef HAVE_X11
  yawl::Descriptor desc;
  desc.setDimensions(yawl::Size<size_t>::create(800, 600).value())
      .setName("Yawl Example Window");
  auto result = yawl::XWindow::create(desc);
  if (result.is_err()) {
    std::cerr << "Failed to create window: " << static_cast<int>(result.error())
              << std::endl;
    return 1;
  }

  yawl::XWindow window = std::move(result.value());
  std::cout << "Window created successfully!" << std::endl;

  eventLoop.mount(std::make_unique<yawl::XWindow>(std::move(window)));
#else
  std::cerr << "X11 support is not enabled. Please compile with X11 support."
            << std::endl;
  return 1;
#endif

  eventLoop.run();

  std::cout << "Event loop has ended." << std::endl;
  return 0;
}