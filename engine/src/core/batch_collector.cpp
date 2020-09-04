#include "batch_collector.h"

using namespace tst;

void BatchCollector::BatchStart() const
{
    assert(_batch.empty());
}

void BatchCollector::AddQuad(const Quad& quad, const glm::mat4& m)
{
    // todo: тут нужен рефакторинг решения
    // оптимизировать умножение на матрицу

    glm::vec4 a(quad.vertices[0].x, quad.vertices[0].y, quad.vertices[0].z, 1.f);
    glm::vec4 b(quad.vertices[1].x, quad.vertices[1].y, quad.vertices[1].z, 1.f);
    glm::vec4 c(quad.vertices[2].x, quad.vertices[2].y, quad.vertices[2].z, 1.f);
    glm::vec4 d(quad.vertices[3].x, quad.vertices[3].y, quad.vertices[3].z, 1.f);

    a = m * a;
    b = m * b;
    c = m * c;
    d = m * d;

    _batch.emplace_back(Vertex(a.x, a.y, a.z, quad.vertices[0].u, quad.vertices[0].v, quad.vertices[0].color));
    _batch.emplace_back(Vertex(b.x, b.y, b.z, quad.vertices[1].u, quad.vertices[1].v, quad.vertices[1].color));
    _batch.emplace_back(Vertex(c.x, c.y, c.z, quad.vertices[2].u, quad.vertices[2].v, quad.vertices[2].color));

    _batch.emplace_back(Vertex(c.x, c.y, c.z, quad.vertices[2].u, quad.vertices[2].v, quad.vertices[2].color));
    _batch.emplace_back(Vertex(d.x, d.y, d.z, quad.vertices[3].u, quad.vertices[3].v, quad.vertices[3].color));
    _batch.emplace_back(Vertex(a.x, a.y, a.z, quad.vertices[0].u, quad.vertices[0].v, quad.vertices[0].color));
}

const void * BatchCollector::BatchData() const
{
    return _batch.data();
}

unsigned int BatchCollector::BatchMemSize() const
{
    return static_cast<unsigned int>(_batch.size() * sizeof(Vertex));
}

unsigned int BatchCollector::BatchSize() const
{
    return static_cast<unsigned int>(_batch.size());
}

void BatchCollector::BatchFinish() const
{
    _batch.clear();
}
