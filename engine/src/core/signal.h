#pragma once

#ifndef TST_SIGNAL_H
#define TST_SIGNAL_H

namespace tst
{
    enum class SignalType
    {
        None,
        MouseDown,
        MouseUp,
        MouseCancel,
        MouseMove,
        MouseWheel,
        KeyPressed,
        KeyReleased
    };

    struct Signal
    {
        SignalType signalType = SignalType::None;
        int key = 0;
        float x = 0.f;
        float y = 0.f;
    };
}

#endif//TST_SIGNAL_H
