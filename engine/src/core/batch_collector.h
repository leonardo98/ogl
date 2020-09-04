#pragma once

#ifndef BATCH_COLLECTOR_H
#define BATCH_COLLECTOR_H

#include "vertex.h"
#include "quad.h"

#include <glm/glm.hpp>

#include <vector>

// системный класс, работает исключительно в главном потоке 

namespace tst
{
    class BatchCollector
    {
    public:
        void BatchStart() const;
        void AddQuad(const Quad &quad, const glm::mat4 &m);
        void BatchFinish() const;
        const void * BatchData() const;
        unsigned int BatchMemSize() const;
        unsigned int BatchSize() const;

    private:
        mutable std::vector<Vertex> _batch;

        friend class Sprite;
    };
}

#endif//BATCH_COLLECTOR_H