#include "vertex.h"

using namespace tst;

Vertex::Vertex()
    : x(0.f)
    , y(0.f)
    , z(-1.f) // marker of not inited vertex
    , u(0.f)
    , v(0.f)
    , color(0xFFFFFFFF)
{
}

Vertex::Vertex(float x_, float y_, float z_, float u_, float v_, unsigned int color_)
    : x(x_)
    , y(y_)
    , z(z_)
    , u(u_)
    , v(v_)
    , color(color_)
{
}