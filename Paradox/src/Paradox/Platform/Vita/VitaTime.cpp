#include "pxpch.h"
#include "Paradox/Core/Time.h"

#include <psp2/kernel/processmgr.h>

namespace Paradox
{
    float Time::GetTime()
    {
        static uint64_t start = 0;

        uint64_t now = sceKernelGetProcessTimeWide();

        if (start == 0)
            start = now;

        return (float)((now - start) * 0.000001);
    }
}