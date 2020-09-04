//
// Variable for accessing from different threads
// 

#pragma once

#ifndef GAME_VAR_H
#define GAME_VAR_H

#include <mutex>

namespace tst
{
    template <class T>
    class GameVar
    {
    public:
        // setting variable
        void SetValue(const T& value)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _incomingValue = value;
        }
        // getting current value of variable
        const T& GetValue() const
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _value = _incomingValue;
            return _value;
        }
        // lock free getting current value of variable
        // returns old value, if variable is locked by another thread
        const T& GetValueLF() const
        {
            if (_mutex.try_lock())
            {
                _value = _incomingValue;
                _mutex.unlock();
            }
            return _value;
        }
    private:
        T _incomingValue;
        mutable T _value;
        mutable std::mutex _mutex;
    };
}

#endif//GAME_VAR_H
