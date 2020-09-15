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
    class Texture : public Actor
    {
    public:

        // методы вызываются из главного потока
        virtual ~Texture();

        // методы вызываются из игрового потока
        Texture(std::weak_ptr<Material> material, const char* fileName);
        int Width() const;
        int Height() const;
        bool Valid() const;

    protected:

        // методы вызываются из главного потока
        void Render(const RenderState& rs) const override;
        void Update(float dt) override;

    private:

        // к этим полям обращаемся из любого потока
        ActorState _state;
        int _width;
        int _height;
        unsigned char *_buffer;
        std::weak_ptr<Material> _material;
        std::shared_ptr<BatchCollector> _batcher;
        mutable std::mutex _mutex;

        // к этим полям обращение может быть только из главного потока
        GLuint _textureHandle;
        GLuint _vertexBuffer;
        GLuint _uniformTextureID;
        GLuint _vertexPosID;
        GLuint _vertexUVID;
        GLuint _vertexAlphaID;
    };
    typedef std::shared_ptr<Texture> SharedTexture;
}

#endif//TST_TEXTURE_H
