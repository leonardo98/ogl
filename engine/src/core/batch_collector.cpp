#include "batch_collector.h"

using namespace tst;

unsigned char Mul(float a, float b)
{
    assert(a >= 0.f && b >= 0.f && a * b <= 1.f);
    return static_cast<unsigned char>(a * b * 255);
}

void BatchCollector::AddQuad(const Quad& quad, const RenderState& rs)
{
    // todo: тут нужен рефакторинг решения
    // оптимизировать умножение на матрицу

    glm::vec4 a(quad.vertices[0].x, quad.vertices[0].y, quad.vertices[0].z, 1.f);
    glm::vec4 b(quad.vertices[1].x, quad.vertices[1].y, quad.vertices[1].z, 1.f);
    glm::vec4 c(quad.vertices[2].x, quad.vertices[2].y, quad.vertices[2].z, 1.f);
    glm::vec4 d(quad.vertices[3].x, quad.vertices[3].y, quad.vertices[3].z, 1.f);

    a = rs.matrix * a;
    b = rs.matrix * b;
    c = rs.matrix * c;
    d = rs.matrix * d;

    _batch.emplace_back(RenderVertex(a.x, a.y, a.z, quad.vertices[0].u, quad.vertices[0].v, Mul(quad.vertices[0].alpha, rs.alpha)));
    _batch.emplace_back(RenderVertex(b.x, b.y, b.z, quad.vertices[1].u, quad.vertices[1].v, Mul(quad.vertices[1].alpha, rs.alpha)));
    _batch.emplace_back(RenderVertex(c.x, c.y, c.z, quad.vertices[2].u, quad.vertices[2].v, Mul(quad.vertices[2].alpha, rs.alpha)));

    _batch.emplace_back(RenderVertex(c.x, c.y, c.z, quad.vertices[2].u, quad.vertices[2].v, Mul(quad.vertices[2].alpha, rs.alpha)));
    _batch.emplace_back(RenderVertex(d.x, d.y, d.z, quad.vertices[3].u, quad.vertices[3].v, Mul(quad.vertices[3].alpha, rs.alpha)));
    _batch.emplace_back(RenderVertex(a.x, a.y, a.z, quad.vertices[0].u, quad.vertices[0].v, Mul(quad.vertices[0].alpha, rs.alpha)));
}

const void * BatchCollector::Data() const
{
    return _batch.data();
}

unsigned int BatchCollector::MemSize() const
{
    return static_cast<unsigned int>(_batch.size() * sizeof(Vertex));
}

unsigned int BatchCollector::Size() const
{
    return static_cast<unsigned int>(_batch.size());
}

void BatchCollector::Clear() const
{
    _batch.clear();
}
