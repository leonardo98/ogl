#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "external\stb_image\stb_image.h"

#include <assert.h>

using namespace tst;

int Texture::Width() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _width;
}

int Texture::Height() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _height;
}

Texture::Texture(std::weak_ptr<Material> material, const char* fileName)
    : _textureHandle(0u)
    , _uniformTextureID(0u)
    , _buffer(nullptr)
    , _state(ActorState::None)
    , _width(0)
    , _height(0)
    , _material(material)
{
    _batcher = std::make_shared<BatchCollector>();
    int comp = 0;
    _buffer = stbi_load(fileName, &_width, &_height, &comp, STBI_default);

    if (!_buffer || _width == 0 || _height == 0)
    {
        //log::write("texture with zero size %s", fileName);
        _state = ActorState::Failed;

        stbi_image_free(_buffer);
        _buffer = nullptr;
        return;
    }
    if (comp != 4/* && comp != 3*/)
    {
        //log::write("not RGBA8888 or RGB888 format %s", fileName);
        _state = ActorState::Failed;

        stbi_image_free(_buffer);
        _buffer = nullptr;
        return;
    }

    _state = ActorState::Loaded;
}

/*
*/

// вызывается только из главного потока
void Texture::Render(const RenderState& rs) const
{
    if (_state == ActorState::Binded)
    {
        assert(_textureHandle);
        assert(_buffer == nullptr);

        // Bind our texture in Texture Unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _textureHandle);
        // Set our "myTextureSampler" sampler to user Texture Unit 0
        glUniform1i(_uniformTextureID, 0);

        //_batcher->BatchStart();

        _batcher->Clear();

        RenderState renderState{rs.matrix, rs.alpha, _batcher.get() };
        Actor::Render(renderState);

        if (_batcher->Size())
        {
            glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
            glBufferData(GL_ARRAY_BUFFER, _batcher->MemSize(), _batcher->Data(), GL_DYNAMIC_DRAW);

            unsigned int baseOffset = 0;

            // 1rst attribute buffer : vertices
            glEnableVertexAttribArray(_vertexPosID);
            glVertexAttribPointer(
                _vertexPosID,                 // The attribute we want to configure
                3,                            // size
                GL_FLOAT,                     // type
                GL_FALSE,                     // normalized?
                sizeof(RenderVertex),               // stride
                reinterpret_cast<void*>(baseOffset + offsetof(RenderVertex, x)) // array buffer offset
            );

            // 2nd attribute buffer : UVs
            glEnableVertexAttribArray(_vertexUVID);
            glVertexAttribPointer(
                _vertexUVID,                  // The attribute we want to configure
                2,                            // size : U+V => 2
                GL_FLOAT,                     // type
                GL_FALSE,                     // normalized?
                sizeof(RenderVertex),               // stride
                reinterpret_cast<void*>(baseOffset + offsetof(RenderVertex, u))    // array buffer offset (skipping x, y, z)
            );

            // 3rd attribute buffer : Colors
            glEnableVertexAttribArray(_vertexAlphaID);
            glVertexAttribPointer(
                _vertexAlphaID,               // The attribute we want to configure
                1,                            // size : alpha is unsigned char
                GL_UNSIGNED_BYTE,             // type
                GL_FALSE,                     // normalized?
                sizeof(RenderVertex),               // stride
                reinterpret_cast<void*>(baseOffset + offsetof(RenderVertex, alpha))    // array buffer offset (skipping x, y, z, u, v)
            );

            // Draw the triangles !
            glDrawArrays(GL_TRIANGLES, 0, 3 * _batcher->Size()); // 12*3 indices starting at 0 -> 12 triangles

            glDisableVertexAttribArray(_vertexPosID);
            glDisableVertexAttribArray(_vertexUVID);
            glDisableVertexAttribArray(_vertexAlphaID);
        }
    }
}

// вызывается только из главного потока
void Texture::Update(float dt)
{
    if (_state == ActorState::Binded)
    {
        assert(_textureHandle);
        assert(_buffer == nullptr);
    }
    else if (_state == ActorState::Loaded)
    {
        if (_mutex.try_lock())
        {
            assert(_textureHandle == 0);
            assert(_buffer != nullptr);
            glGenTextures(1, &_textureHandle);

            glBindTexture(GL_TEXTURE_2D, _textureHandle);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _buffer);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

            glBindTexture(GL_TEXTURE_2D, 0);

            stbi_image_free(_buffer);
            _buffer = nullptr;
            _state = ActorState::Binded;

            if (auto m = _material.lock())
            {
                auto programID = m->GetProgramID();
                if (programID)
                {
                    _uniformTextureID = glGetUniformLocation(programID, "myTextureSampler");

                    // Get a handle for our buffers
                    _vertexPosID = glGetAttribLocation(programID, "vertexPosition_modelspace");
                    _vertexUVID = glGetAttribLocation(programID, "vertexUV");
                    _vertexAlphaID = glGetAttribLocation(programID, "vertexAlpha");
                }
            }

            glGenBuffers(1, &_vertexBuffer);

            _mutex.unlock();
        }
    }
    else
    {
        assert(_textureHandle == 0);
        assert(_buffer == nullptr);
    }

    Actor::Update(dt);
}

bool Texture::Valid() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _state == ActorState::Loaded || _state == ActorState::Binded;
}

Texture::~Texture()
{
    if (_textureHandle)
    {
        assert(_state == ActorState::Binded);
        assert(_buffer == nullptr);
        glDeleteBuffers(1, &_vertexBuffer);
        glDeleteTextures(1, &_textureHandle);
        return;
    }
    if (_buffer)
    {
        assert(_state == ActorState::Loaded);
        assert(_textureHandle == 0);
        stbi_image_free(_buffer);
        _buffer = nullptr;
        return;
    }
    assert(_state == ActorState::Failed);
    assert(_buffer == nullptr);
    assert(_textureHandle == 0);
}
