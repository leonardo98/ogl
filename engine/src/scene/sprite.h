#pragma once

#ifndef TST_SPRITE_H
#define TST_SPRITE_H

#include "core/quad.h"

#include "actor.h"
#include "texture.h"

namespace tst
{
    class Sprite : public Actor
    {
    public:
        // методы вызываются из игрового потока
        Sprite(std::weak_ptr<Texture> texture, int x, int y, int width, int height);

    protected:
        // методы вызываются из главного потока
        virtual void Render(const glm::mat4& m) const override;
        
    private:
        // к полям вызываются из любого потока, но не конкурентно, mutex не нужен
        Quad _quad;
        float _width;
        float _height;
        std::weak_ptr<Texture> _texture;
    };
}

#endif//SPRITE_H
