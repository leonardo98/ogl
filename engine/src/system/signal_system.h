#pragma once

#ifndef SIGNAL_SYSTEM_H
#define SIGNAL_SYSTEM_H

#include "core/signal.h"

#include <list>
#include <mutex>

namespace tst
{
    class SignalSystem
    {
    public:
        // обращаемся к методам из любого потока
        void AddSignal(const SharedSignal& signal);
        bool IsEmpty() const;
        SharedSignal PopSignal();
        void Clear();

        static SignalSystem* Instance();

    private:
        static SignalSystem* _instance;
        static std::mutex _instanceMutex;
        std::list<SharedSignal> _signals;

        SignalSystem();
        mutable std::mutex _mutex;
    };
}

#endif//SIGNAL_SYSTEM_H