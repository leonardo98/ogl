#pragma once

#ifndef TST_TEXTURE_H
#define TST_TEXTURE_H

#include "core/actor_state.h"
#include "core/batch_collector.h"

#include "actor.h"
#include "material.h"

#include <GL/glew.h>

#include <vector>
#include <memory>
#include <mutex>

namespace tst
{
    class Texture : public Actor, protected BatchCollector
    {
    public:

        // ������ ���������� �� �������� ������
        virtual ~Texture();

        // ������ ���������� �� �������� ������
        Texture(std::weak_ptr<Material> material, const char* fileName);
        int Width() const;
        int Height() const;
        bool Valid() const;

    protected:

        // ������ ���������� �� �������� ������
        void Render(const glm::mat4& m) const override;
        void Update(float dt) override;

    private:

        // � ���� ����� ���������� �� ������ ������
        ActorState _state;
        int _width;
        int _height;
        unsigned char *_buffer;
        std::weak_ptr<Material> _material;
        mutable std::mutex _mutex;

        // � ���� ����� ��������� ����� ���� ������ �� �������� ������
        GLuint _textureHandle;
        GLuint _vertexBuffer;
        GLuint _uniformTextureID;
        GLuint _vertexPosID;
        GLuint _vertexUVID;
    };
    typedef std::shared_ptr<Texture> SharedTexture;
}

#endif//TST_TEXTURE_H
