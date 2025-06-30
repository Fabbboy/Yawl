#pragma once

#include <cstdint>

constexpr uint16_t clampToU16(uint32_t value) {
  return static_cast<uint16_t>(value > UINT16_MAX ? UINT16_MAX : value);
}
