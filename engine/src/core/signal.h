#pragma once

#ifndef TST_SIGNAL_H
#define TST_SIGNAL_H

#include <memory>

namespace tst
{
    struct SignalBase
    {
        unsigned int code;
    };

    template <class T>
    struct Signal : public SignalBase
    {
        T value;
    };

    typedef std::shared_ptr<SignalBase> SharedSignal;
}

#endif//TST_SIGNAL_H
