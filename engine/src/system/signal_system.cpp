#include "signal_system.h"

using namespace tst;

SignalSystem* SignalSystem::_instance = nullptr;
std::mutex SignalSystem::_instanceMutex;

void SignalSystem::AddSignal(const SharedSignal& signal)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _signals.push_back(signal);
}

bool SignalSystem::IsEmpty() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _signals.empty();
}

SharedSignal SignalSystem::PopSignal()
{
    std::lock_guard<std::mutex> lock(_mutex);
    if (_signals.empty())
    {
        return nullptr;
    }

    SharedSignal signal = _signals.front();
    _signals.pop_front();
    return signal;
}

void SignalSystem::Clear()
{
    std::lock_guard<std::mutex> lock(_mutex);
    _signals.clear();
}

SignalSystem* SignalSystem::Instance()
{
    std::lock_guard<std::mutex> lock(_instanceMutex);
    if (_instance == nullptr)
    {
        _instance = new SignalSystem();
    }
    return _instance;
}

SignalSystem::SignalSystem()
{
}
