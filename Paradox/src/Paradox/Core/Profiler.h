#pragma once

#define PX_ENABLE_PROFILER (!PX_DIST && PX_INCLUDE_TRACY)

#if PX_ENABLE_PROFILER
#include <tracy/Tracy.hpp>
#define PX_PROFILE_FUNCTION() ZoneScoped
#define PX_PROFILE_SCOPE(name) ZoneScopedN(name)
#define PX_PROFILE_FRAME_MARK() FrameMark
#else
#define PX_PROFILE_FUNCTION()
#define PX_PROFILE_SCOPE(name)
#define PX_PROFILE_FRAME_MARK()
#endif
