#pragma once

#include <cstdio>
#include <cstdlib>
namespace yawl {
[[noreturn]]
inline void panic(const char *message, const char *file, int line) {
  std::fprintf(stderr, "PANIC at %s:%d - %s\n", file, line, message);
  std::abort();
}

[[noreturn]]
inline void unreachable(const char *file, int line) {
  panic("Unreachable code reached", file, line);
}

[[noreturn]]
inline void unimplemented(const char *file, int line) {
  panic("Unimplemented code reached", file, line);
}

#define PANIC(msg) ::yawl::panic(msg, __FILE__, __LINE__)
#define UNREACHABLE() ::yawl::unreachable(__FILE__, __LINE__)
#define UNIMPLEMENTED() ::yawl::unimplemented(__FILE__, __LINE__)

} // namespace yawl