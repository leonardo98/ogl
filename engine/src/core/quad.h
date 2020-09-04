#pragma once

#ifndef QUAD_H
#define QUAD_H

#include "vertex.h"

#include <memory>

namespace tst
{
    struct Quad
    {
        Vertex vertices[4];
    };
}

#endif//QUAD_H
