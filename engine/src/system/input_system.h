#pragma once

#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "core/signal.h"

#include <list>
#include <mutex>

namespace tst
{
    class InputSystem
    {
    public:
        // обращаемся к методам из любого потока
        void AddSignal(const Signal &signal);
        bool IsEmpty() const;
        Signal PopSignal();
        static InputSystem* Instance();

    private:
        static InputSystem* _instance;
        std::list<Signal> _signals;
        InputSystem();
        mutable std::mutex _mutex;
    };
}

#endif//INPUT_MANAGER_H