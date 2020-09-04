#include "input_system.h"

using namespace tst;

InputSystem* InputSystem::_instance = nullptr;
std::mutex _instanceMutex;

void InputSystem::AddSignal(const Signal &signal)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _signals.push_back(signal);
}

bool InputSystem::IsEmpty() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _signals.empty();
}

Signal InputSystem::PopSignal()
{
    std::lock_guard<std::mutex> lock(_mutex);
    if (_signals.empty())
    {
        return Signal();
    }

    Signal signal = _signals.front();
    _signals;
    return signal;
}

InputSystem* InputSystem::Instance()
{
    std::lock_guard<std::mutex> lock(_instanceMutex);
    if (_instance == nullptr)
    {
        _instance = new InputSystem();
    }
    return _instance;
}

InputSystem::InputSystem()
{
}
