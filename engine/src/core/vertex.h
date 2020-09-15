#pragma once

#ifndef TST_VERTEX_H
#define TST_VERTEX_H

namespace tst
{
    // ������� ��� ������������� � ������
    struct Vertex
    {
        float x, y, z;      // point in 3D
        float u, v;         // texture's coordinates
        float alpha;

        Vertex();
        Vertex(float x, float y, float z, float u, float v, float alpha);
    };

    // ������� ��� ������������� � ������� ������ OpenGL
    struct RenderVertex
    {
        float x, y, z;      // point in 3D
        float u, v;         // texture's coordinates
        unsigned char alpha;

        RenderVertex();
        RenderVertex(float x, float y, float z, float u, float v, unsigned char alpha);
    };
}

#endif//TST_VERTEX_H