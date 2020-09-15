#pragma once

#ifndef BATCH_COLLECTOR_H
#define BATCH_COLLECTOR_H

#include "vertex.h"
#include "quad.h"
#include "render_state.h"

#include <vector>

// системный класс, работает исключительно в главном потоке 

namespace tst
{
    class BatchCollector
    {
    public:
        void AddQuad(const Quad &quad, const RenderState &rs);
        void Clear() const;
        const void * Data() const;
        unsigned int MemSize() const;
        unsigned int Size() const;

    private:
        mutable std::vector<RenderVertex> _batch;

        friend class Sprite;
    };
}

#endif//BATCH_COLLECTOR_H