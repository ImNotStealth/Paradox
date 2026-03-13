#include <Paradox.h>

int main()
{
    Paradox::Log::Init();
    PX_INFO("Hello!");
    PX_WARN("Hello!");
    PX_ERROR("Hello!");
    PX_CRITICAL("Hello!");
	PX_TRACE("Hello!");

    PX_CORE_INFO("Hello!");
    PX_CORE_WARN("Hello!");
    PX_CORE_ERROR("Hello!");
    PX_CORE_CRITICAL("Hello!");
    PX_CORE_TRACE("Hello!");
    return 0;
}