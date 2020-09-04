#include "sprite.h"

using namespace tst;

Sprite::Sprite(std::weak_ptr<Texture> texture, int x, int y, int width, int height)
    : _texture(texture)
    , _width(static_cast<float>(width))
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

    _quad.vertices[0].color = _quad.vertices[1].color = _quad.vertices[2].color = _quad.vertices[3].color = 0xFFFFFFFF;
}

void Sprite::Render(const glm::mat4& m) const
{
    glm::mat4 result = m * _local;
    if (auto sp = _texture.lock())
    {
        auto bc = (BatchCollector*)(sp.get());
        bc->AddQuad(_quad, result);
    }
}
