#pragma once

#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

//#include "core/signal.h"

#include <glm/glm.hpp>

#include <list>
#include <mutex>

namespace tst
{
    class InputSystem
    {
    public:
        // обращаемся к методам из любого потока
        //void AddSignal(const Signal &signal);
        //bool IsEmpty() const;
        //Signal PopSignal();
        //void Clear();

        void SetMousePos(float x, float y);
        const glm::vec3 &GetMousePos() const;
        void SetMousePressed(bool pressed);
        bool GetMousePressed() const;

        static InputSystem* Instance();

    private:
        static InputSystem* _instance;
        static std::mutex _instanceMutex;
        //std::list<Signal> _signals;

        bool _isMousePressed;
        glm::vec3 _mousePos;

        InputSystem();
        mutable std::mutex _mutex;
    };
}

#endif//INPUT_MANAGER_H