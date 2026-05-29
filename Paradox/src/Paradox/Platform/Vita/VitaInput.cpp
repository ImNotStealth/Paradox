#include "pxpch.h"
#include "Paradox/Core/Input.h"

#include <psp2/ctrl.h>

namespace Paradox
{
    bool Input::IsKeyPressed(KeyCode keyCode)
    {
        sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);

        SceCtrlData ctrl;
        sceCtrlPeekBufferPositive(0, &ctrl, 1);

        const float ly = ((ctrl.ly - 128) / 256.0f) * 2;
        const float lx = ((ctrl.lx - 128) / 256.0f) * 2;

        const float ry = ((ctrl.ry - 128) / 256.0f) * 2;
        const float rx = ((ctrl.rx - 128) / 256.0f) * 2;

        switch (keyCode)
        {
            case Keyboard::KP0: return ctrl.buttons & SCE_CTRL_CROSS;
            case Keyboard::KP1: return ctrl.buttons & SCE_CTRL_CIRCLE;
            case Keyboard::KP2: return ctrl.buttons & SCE_CTRL_TRIANGLE;
            case Keyboard::KP3: return ctrl.buttons & SCE_CTRL_SQUARE;

            case Keyboard::W: return ly < -0.1f;
            case Keyboard::S: return ly > 0.1f;
            case Keyboard::A: return lx < -0.1f;
            case Keyboard::D: return lx > 0.1f;

            case Keyboard::Up: return ry < -0.1f;
            case Keyboard::Down: return ry > 0.1f;
            case Keyboard::Left: return rx < -0.1f;
            case Keyboard::Right: return rx > 0.1f;

            case Keyboard::Q: return ctrl.buttons & SCE_CTRL_LTRIGGER;
            case Keyboard::E: return ctrl.buttons & SCE_CTRL_RTRIGGER;
        }
        return false;
    }
}