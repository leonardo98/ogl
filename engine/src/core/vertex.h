#pragma once

#ifndef TST_VERTEX_H
#define TST_VERTEX_H

namespace tst
{
    struct Vertex
    {
        float x, y, z;      // point in 3D
        float u, v;         // texture's coordinates
        unsigned int color;

        Vertex();
        Vertex(float x, float y, float z, float u, float v, unsigned int color);
    };
}

#endif//TST_VERTEX_H