#pragma once

#ifndef TST_BUTTON_H
#define TST_BUTTON_H

#include "system/actors/mouse_area.h"

#include <functional>

namespace tst
{
    enum class ButtonState
    {
        Idle,
        Hover,
        Pressed,
        Locked
    };

    class Button : public MouseArea, public std::enable_shared_from_this<Button>
    {
    public:
        // методы вызываются из игрового потока
        Button(int width, int height, const std::function<void(std::shared_ptr<Button>)> &onPressedCallback);

    protected:

        virtual void OnMouseIn() override;
        virtual void OnMouseOut() override;

        virtual void Update(float dt) override;

    private:

        void SetState(ButtonState state);
        ButtonState _state = ButtonState::Idle;
        std::function<void(std::shared_ptr<Button>)> _onPressedCallback;

    };
}

#endif//TST_BUTTON_H
