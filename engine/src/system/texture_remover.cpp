#include "texture_remover.h"

using namespace tst;

TextureRemover* TextureRemover::_instance = nullptr;
std::mutex _instanceMutex;

void TextureRemover::AddTexture(GLuint textureHandle)
{
	std::lock_guard<std::mutex> lock(_mutex);
	_textures.push_back(textureHandle);
}

void TextureRemover::Remove()
{
	if (_mutex.try_lock())
	{
		if (_textures.size())
		{
			glDeleteTextures((GLsizei)_textures.size(), _textures.data());
			_textures.clear();
		}
		_mutex.unlock();
	}
}

TextureRemover* TextureRemover::Instance()
{
	std::lock_guard<std::mutex> lock(_instanceMutex);
	if (_instance == nullptr)
	{
		_instance = new TextureRemover();
	}
	return _instance;
}

TextureRemover::TextureRemover()
{
}

