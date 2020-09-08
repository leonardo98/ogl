#pragma once

#ifndef DEBUG_RENDER_H
#define DEBUG_RENDER_H

#include "core/quad.h"

#include <glm/glm.hpp>

#include <vector>
#include <mutex>

namespace tst
{
    class DebugRender
    {
    public:
        void AddRect(const glm::vec4 &a, const glm::vec4 &b, const glm::vec4 &c, const glm::vec4 &d);

        const void* BatchData() const;
        unsigned int BatchMemSize() const;
        unsigned int BatchSize() const;
        void Clear();

        static DebugRender* Instance();

    private:
        static DebugRender* _instance;
        static std::mutex _instanceMutex;
        std::vector<glm::vec3> _points;

        DebugRender();
    };
}

#endif//DEBUG_RENDER_H