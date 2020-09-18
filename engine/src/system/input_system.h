#pragma once

#ifndef INPUT_SYSTEM_H
#define INPUT_SYSTEM_H

#include <glm/glm.hpp>

#include <list>
#include <mutex>

namespace tst
{
    class InputSystem
    {
    public:

        void SetMousePos(float x, float y);
        const glm::vec3 &GetMousePos() const;
        void SetMousePressed(bool pressed);
        bool GetMousePressed() const;

        static InputSystem* Instance();

    private:
        static InputSystem* _instance;
        static std::mutex _instanceMutex;

        bool _isMousePressed;
        glm::vec3 _mousePos;

        InputSystem();
        mutable std::mutex _mutex;
    };
}

#endif//INPUT_SYSTEM_H