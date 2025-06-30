#pragma once

#include "Size.h"
#include <cstddef>
#include <optional>
#include <string_view>

namespace yawl {
struct Descriptor {
public:
  std::optional<int> screen;
  Size<size_t> dimensions;
  std::string_view name;

public:
  Descriptor() = default;

  inline Descriptor &setScreen(int scr) {
    screen = scr;
    return *this;
  }

  inline Descriptor &setDimensions(const Size<size_t> &dim) {
    dimensions = dim;
    return *this;
  }

  inline Descriptor &setName(std::string_view n) {
    name = n;
    return *this;
  }
};

} // namespace yawl