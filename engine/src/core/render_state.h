#pragma once

#ifndef RENDER_STATE_H
#define RENDER_STATE_H

#include <glm/glm.hpp>

namespace tst
{
	class BatchCollector;

	struct RenderState
	{
		glm::mat4 matrix;
		float alpha;
		BatchCollector* batch = nullptr;

		// texture
		// material
	};
}

#endif//RENDER_STATE_H