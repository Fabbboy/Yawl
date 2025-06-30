#pragma once

#include "Utility/Size.h"
#include "Utility/Result.h"
#include "Utility/Value.h"

#include "Windowing/Descriptor.h"
#include "Windowing/Window.h"
#include "Windowing/RawHandle.h"
#ifdef HAVE_X11
#include "Windowing/XWindow.h"
#include "Windowing/XClient.h"
#endif

#include "Event/Event.h"
#include "Event/Loop.h"
#include "Event/Handler.h"
