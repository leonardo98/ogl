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
        MouseMove,
        MouseWheel,
        KeyPressed,
        KeyReleased
    };

    struct Signal
    {
        SignalType signalType = SignalType::None;
        int key = 0;
    };
}

#endif//TST_SIGNAL_H
