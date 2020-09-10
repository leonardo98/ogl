#include "button.h"

#include "system/input_system.h"

using namespace tst;

Button::Button(int width, int height, const std::function<void(std::shared_ptr<Button>)>& onPressedCallback)
	: MouseArea(width, height)
	, _state(ButtonState::Idle)
	, _onPressedCallback(onPressedCallback)
{
}

void Button::OnMouseIn()
{
	if (_state == ButtonState::Idle)
	{
		if (!InputSystem::Instance()->GetMousePressed()) // not locked by pressing outside ?
		{
			SetState(ButtonState::Hover);
		}
	}
	else if (_state == ButtonState::Locked)
	{
		assert(InputSystem::Instance()->GetMousePressed());
		SetState(ButtonState::Pressed);
	}
	else if (_state == ButtonState::Hover || _state == ButtonState::Pressed)
	{
		assert(false);
	}
	else
	{
		assert(false);
	}
}

void Button::OnMouseOut()
{
	if (_state == ButtonState::Idle)
	{
	}
	else if (_state == ButtonState::Pressed)
	{
		assert(InputSystem::Instance()->GetMousePressed());
		SetState(ButtonState::Locked);
	}
	else if (_state == ButtonState::Locked)
	{
		assert(false);
	}
	else if (_state == ButtonState::Hover)
	{
		assert(!InputSystem::Instance()->GetMousePressed());
		SetState(ButtonState::Idle);
	}
	else
	{
		assert(false);
	}
}

void Button::SetState(ButtonState state)
{
	assert(_state != state);

	if (_state == ButtonState::Pressed && state == ButtonState::Hover)
	{
		if (_onPressedCallback)
		{
			_onPressedCallback(shared_from_this());
		}
	}

	if (state == ButtonState::Pressed)
	{
		for (auto& c : _children)
		{
			c->CreateTween<TweenScale>(0.1f, 0.85f);
		}
	}
	else if (state == ButtonState::Hover)
	{
		for (auto& c : _children)
		{
			c->CreateTween<TweenScale>(0.2f, 1.1f);
		}
	}
	else if (state == ButtonState::Locked || state == ButtonState::Idle)
	{
		for (auto& c : _children)
		{
			c->CreateTween<TweenScale>(0.2f, 1.f);
		}
	}

	_state = state;
}

void Button::Update(float dt)
{
	MouseArea::Update(dt);

	if (_state == ButtonState::Hover && InputSystem::Instance()->GetMousePressed())
	{
		assert(IsMouseIn());

		SetState(ButtonState::Pressed);
	}
	else if (_state == ButtonState::Pressed && !InputSystem::Instance()->GetMousePressed())
	{
		assert(IsMouseIn());

		SetState(ButtonState::Hover);
	}
	else if (_state == ButtonState::Locked && !InputSystem::Instance()->GetMousePressed())
	{
		assert(!IsMouseIn());

		SetState(ButtonState::Idle);
	}
	else if (_state == ButtonState::Idle)
	{
		// nothing to do
	}
}
