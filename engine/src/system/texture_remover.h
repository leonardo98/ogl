#pragma once

#ifndef TEXTURE_REMOVER_H
#define TEXTURE_REMOVER_H

#include <GL/glew.h>

#include <mutex>
#include <vector>

namespace tst
{
	class TextureRemover
	{
	public:
		void AddTexture(GLuint textureHandle);
		void Remove();
		static TextureRemover* Instance();
	private:
		static TextureRemover* _instance;
		std::vector<GLuint> _textures;
		TextureRemover();
		std::mutex _mutex;
	};
}

#endif//TEXTURE_REMOVER_H