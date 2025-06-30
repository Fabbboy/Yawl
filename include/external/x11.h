#ifdef HAVE_X11
#include <xcb/xcb.h>
#else
typedef struct {
} xcb_connection_t;
typedef struct {
} xcb_window_t;
#endif
