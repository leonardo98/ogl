#pragma once

#ifndef TST_MATERIAL_H
#define TST_MATERIAL_H

#include "core/actor_state.h"
#include "core/game_var.h"

#include "actor.h"

#include <GL/glew.h>

#include <string>
#include <mutex>

namespace tst
{
    class Material : public Actor
    {
    public:
        // методы вызываются из главного потока
        ~Material();
        GLuint GetProgramID() const;

        // методы вызываются из игрового потока
        Material(const char * vertexFilePath,const char * fragmentFilePath);
        void SetMatrix(const glm::mat4 &matrix);

    protected:
        // методы вызываются из главного потока
        virtual void Render(const RenderState& rs) const override;
        void Update(float dt) override;

    private:
        // методы вызываются из главного потока
        void Bind();

        // к этим полям обращаемся из разных потоков
        ActorState _state;

        std::string _vertexShaderCode;
        std::string _fragmentShaderCode;
        GameVar<glm::mat4> _matrix;

        mutable std::mutex _mutex;

        // к этим полям обращение может быть только из главного потока
        GLuint _programID;
        GLuint _matrixID;
    };
}

#endif//TST_MATERIAL_H
