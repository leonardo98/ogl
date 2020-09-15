#pragma once

#ifndef TST_MOUSE_AREA_H
#define TST_MOUSE_AREA_H

#include "scene/actor.h"

#include <vector>

namespace tst
{
    class MouseArea : public Actor
    {
    public:
        // методы вызываются из игрового потока
        MouseArea(int width, int height);

    protected:
        // методы вызываются из главного потока
        virtual void Render(const RenderState& rs) const override;
        virtual void Update(float dt) override;

        virtual void OnMouseIn() {}
        virtual void OnMouseOut() {}

        bool IsMouseIn() const;

    private:

        bool Inside(const glm::vec3& m, const std::vector<glm::vec4>& dots) const;

        // к полям обращаются из главного потока
        float _width;
        float _height;

        mutable std::vector<glm::vec4> _renderableRect;

        bool _isMouseIn;
    };
}

#endif//TST_MOUSE_AREA_H
