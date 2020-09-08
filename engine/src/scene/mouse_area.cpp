#include "mouse_area.h"

#include "system/input_system.h"

#ifdef DEBUG_RENDER
#include "system/debug_render.h"
#endif//DEBUG_RENDER

using namespace tst;

MouseArea::MouseArea(int width, int height)
    : _width(static_cast<float>(width))
    , _height(static_cast<float>(height))
    , _isMouseIn(false)
{
}

void MouseArea::Render(const glm::mat4& m) const
{
    glm::mat4 result = m * _local;

	_renderableRect.clear();
	_renderableRect.emplace_back(glm::vec4(-_width / 2.f, -_height / 2.f, 0.f, 1.f));
	_renderableRect.emplace_back(glm::vec4( _width / 2.f, -_height / 2.f, 0.f, 1.f));
	_renderableRect.emplace_back(glm::vec4( _width / 2.f,  _height / 2.f, 0.f, 1.f));
	_renderableRect.emplace_back(glm::vec4(-_width / 2.f,  _height / 2.f, 0.f, 1.f));
	for (auto& a : _renderableRect)
	{
		a = result * a;
	}
#ifdef DEBUG_RENDER
    DebugRender::Instance()->AddRect(_renderableRect[0], _renderableRect[1], _renderableRect[2], _renderableRect[3]);
#endif//DEBUG_RENDER

    Actor::RenderChild(result);
}

void MouseArea::Update(float dt)
{
	glm::vec3 mousePos = InputSystem::Instance()->GetMousePos();
	bool oldMouseInValue = _isMouseIn;
	_isMouseIn = Inside(mousePos, _renderableRect);
	if (!oldMouseInValue && _isMouseIn)
	{
		OnMouseIn();
	}
	else if (oldMouseInValue && !_isMouseIn)
	{
		OnMouseOut();
	}
}

bool MouseArea::Inside(const glm::vec3& m, const std::vector<glm::vec4>& dots) const
{
	// проверим находиться ли точка внтури области
	int counter = 0;
	const size_t n = dots.size();
	for (size_t j = 0; j < n; ++j)
	{
		const glm::vec4 &a2 = dots[j];
		const glm::vec4 &b2 = dots[(j + 1) % n];
		if ((a2.x < b2.x && a2.x < m.x && m.x <= b2.x)
			|| (a2.x > b2.x && a2.x >= m.x && m.x > b2.x)
			)
		{
			// найти точку пересечения луча из М и отрезка a2b2
			float k = (a2.y - b2.y) / (a2.x - b2.x);
			float b = a2.y - a2.x * k;
			float y = k * m.x + b;
			if (y > m.y)
			{
				++counter;
			}
		}
	}
	return (counter % 2 == 1);
}