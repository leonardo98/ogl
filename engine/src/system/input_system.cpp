#include "input_system.h"

using namespace tst;

InputSystem* InputSystem::_instance = nullptr;
std::mutex InputSystem::_instanceMutex;

//void InputSystem::AddSignal(const Signal &signal)
//{
//    std::lock_guard<std::mutex> lock(_mutex);
//    _signals.push_back(signal);
//}
//
//bool InputSystem::IsEmpty() const
//{
//    std::lock_guard<std::mutex> lock(_mutex);
//    return _signals.empty();
//}
//
//Signal InputSystem::PopSignal()
//{
//    std::lock_guard<std::mutex> lock(_mutex);
//    if (_signals.empty())
//    {
//        return Signal();
//    }
//
//    Signal signal = _signals.front();
//    _signals;
//    return signal;
//}
//
//void InputSystem::Clear()
//{
//    std::lock_guard<std::mutex> lock(_mutex);
//    _signals.clear();
//}

void InputSystem::SetMousePos(float x, float y)
{
    _mousePos = glm::vec3(x, y, 0.f);
}

const glm::vec3 &InputSystem::GetMousePos() const
{
    return _mousePos;
}

void InputSystem::SetMousePressed(bool pressed)
{
    _isMousePressed = pressed;
}

bool InputSystem::GetMousePressed() const
{
    return _isMousePressed;
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
