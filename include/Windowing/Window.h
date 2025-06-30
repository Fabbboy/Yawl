#pragma once

#include "RawHandle.h"
namespace yawl {
struct Window : public HasWindowHandle {
public:
  virtual ~Window() = default;
};
} // namespace yawl