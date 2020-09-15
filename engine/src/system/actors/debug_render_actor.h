#pragma once

#ifndef TST_DEBUG_RENDER_ACTOR_H
#define TST_DEBUG_RENDER_ACTOR_H

#include "core/actor_state.h"
#include "core/batch_collector.h"
#include "core/game_var.h"

#include "scene/actor.h"

#include <GL/glew.h>

#include <vector>
#include <memory>
#include <mutex>

namespace tst
{
    class DebugRenderActor : public Actor, protected BatchCollector
    {
    public:

        // методы вызываются из главного потока
        virtual ~DebugRenderActor();

        // методы вызываются из игрового потока
        DebugRenderActor();
        bool Valid() const;
        void SetMatrix(const glm::mat4& matrix);

    protected:

        // методы вызываются из главного потока
        void Render(const RenderState& rs) const override;
        void Update(float dt) override;

    private:

        void Bind();

        // к этим полям обращаемся из любого потока
        ActorState _state;
        GameVar<glm::mat4> _matrix;
        mutable std::mutex _mutex;

        // к этим полям обращение может быть только из главного потока
        GLuint _vertexBuffer;
        GLuint _vertexPosID;
        GLuint _programID;
        GLuint _matrixID;
    };
}

#endif//TST_DEBUG_RENDER_ACTOR_H
