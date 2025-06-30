#include "Event/Loop.h"
#include <Yawl.h>
#include <iostream>

int main() {
  yawl::EventLoop eventLoop;

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

  while (true) {
  }
#else
  std::cerr << "X11 support is not enabled. Please compile with X11 support."
            << std::endl;
  return 1;
#endif
}