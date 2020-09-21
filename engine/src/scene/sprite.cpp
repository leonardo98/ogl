#include "sprite.h"

using namespace tst;

Sprite::Sprite(std::weak_ptr<Texture> texture, int x, int y, int width, int height)
    : _width(static_cast<float>(width))
    , _height(static_cast<float>(height))
{
    auto sp = texture.lock();
    float w = static_cast<float>(sp->Width());
    float h = static_cast<float>(sp->Height());

    float half_width = width / 2.f;
    float half_height = height / 2.f;

    _quad.vertices[0].x = -half_width; _quad.vertices[0].y = -half_height; _quad.vertices[0].z = 0.f;
    _quad.vertices[1].x = +half_width; _quad.vertices[1].y = -half_height; _quad.vertices[1].z = 0.f;
    _quad.vertices[2].x = +half_width; _quad.vertices[2].y = +half_height; _quad.vertices[2].z = 0.f;
    _quad.vertices[3].x = -half_width; _quad.vertices[3].y = +half_height; _quad.vertices[3].z = 0.f;

    _quad.vertices[0].u = x / w;             _quad.vertices[0].v = y / h;
    _quad.vertices[1].u = (x + _width) / w;  _quad.vertices[1].v = y / h;
    _quad.vertices[2].u = (x + _width) / w;  _quad.vertices[2].v = (y + _height) / h;
    _quad.vertices[3].u = x / w;             _quad.vertices[3].v = (y + _height) / h;

    _quad.vertices[0].alpha = _quad.vertices[1].alpha = _quad.vertices[2].alpha = _quad.vertices[3].alpha = 1.f;
}

void Sprite::Render(const RenderState& rs) const
{
    RenderState result{ rs.matrix * _local, rs.alpha * GetAlpha(), rs.batch };
    assert(rs.batch);
    if (rs.batch)
    {
        rs.batch->AddQuad(_quad, result);
    }
    Actor::RenderChild(result);
}
