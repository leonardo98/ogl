#include "actor.h"

#include <glm/gtc/matrix_transform.inl>

using namespace tst;

std::atomic<unsigned int> _g_actor_counter = 0;
const unsigned int MAX_ACTORS_AMOUNT = 10000;

unsigned int tst::GetActorsAmount()
{
    return _g_actor_counter;
}

// вызывается из игрового потока

Actor::~Actor()
{
    assert(_g_actor_counter > 0);
    _g_actor_counter--;
}

Actor::Actor()
    : _position(0.f, 0.f, 0.f)
    , _angle(0.f)
    , _scale(1.f, 1.f, 1.f)
    , _alpha(1.f)
{
    assert(_g_actor_counter < MAX_ACTORS_AMOUNT);
    _g_actor_counter++;
}

// вызывается из игрового потока
void Actor::AddChild(const std::shared_ptr<Actor>& actor)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _newChildren.push_back(actor);
}

// вызывается из игрового потока
void Actor::RemoveChild(const std::shared_ptr<Actor>& actor)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _childrenToRemove.push_back(actor);
}

// вызывается из игрового потока
void Actor::SetPosition2D(float x, float y, bool forcedLock /*= true*/)
{
    SetPosition(glm::vec3(x, y, 0.f));
}

// вызывается из игрового потока
void Actor::SetRotation2D(float angle, bool forcedLock /*= true*/)
{
    if (forcedLock)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _angle = angle;
    }
    else
    {
        if (_mutex.try_lock())
        {
            _angle = angle;
            _mutex.unlock();
        }
    }
}

// вызывается из игрового потока
void Actor::SetScale2D(float sx, float sy, bool forcedLock /*= true*/)
{
    SetScale(glm::vec3(sx, sy, 1.f));
}

void Actor::SetPosition(const glm::vec3& pos, bool forcedLock /*= true*/)
{
    if (forcedLock)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _position = pos;
    }
    else
    {
        if (_mutex.try_lock())
        {
            _position = pos;
            _mutex.unlock();
        }
    }
}

void Actor::SetScale(const glm::vec3& scale, bool forcedLock /*= true*/)
{
    if (forcedLock)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _scale = scale;
    }
    else
    {
        if (_mutex.try_lock())
        {
            _scale = scale;
            _mutex.unlock();
        }
    }
}

void Actor::SetAlpha(float alpha)
{
    _alpha.SetValue(alpha);
}

const glm::vec3 Actor::GetPosition() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _position;
}

const glm::vec3 Actor::GetScale() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _scale;
}

float Actor::GetRotation2D() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _angle;
}

float Actor::GetAlpha() const
{
    return _alpha.GetValueLF(); // _alpha сам заботится о потоках
}

// методы Render & Update вызывает основной поток, их не надо вызывать самому
void Actor::Render(const RenderState& rs) const
{
    RenderChild({ rs.matrix * _local, rs.alpha, rs.batch });
}

void Actor::RenderChild(const RenderState& rs) const
{
    for (auto& cld : _children)
    {
        cld->Render(rs);
    }
}


// методы Render & Update вызывает основной поток, их не надо вызывать самому
// в основном потоке запрет на использование std::mutex::lock(),
// ничто не должно блокировать работу основного потока,
// он сам заботится о корректном отображении информации,
// в худшем случае она(информация) может устареть на один кадр
void Actor::Update(float dt)
{
    bool hasTweens = TweenPlayer::Update(this, dt);
    
    if (_mutex.try_lock())
    {
        if (_newChildren.size())
        {
            // get new children
            _children.insert(_children.end(), _newChildren.begin(), _newChildren.end());
            _newChildren.clear();
        }
        if (_childrenToRemove.size())
        {
            // remove children
            for (auto& a : _childrenToRemove)
            {
                _children.remove(a);
            }
            _childrenToRemove.clear();
        }

        // todo: тут нужен рефакторинг решения
        // оптимизировать применение трансормаций, если они не оказывают влияние(нулевое смещение или вращение, единичный скейл)

        // update local transformation
        _local = glm::mat4(1.0f);
        _local = glm::translate(_local, _position);
        _local = glm::rotate(_local, _angle, glm::vec3(0.f, 0.f, 1.f));
        _local = glm::scale(_local, _scale);

        _mutex.unlock();
    }

    for (auto& cld : _children)
    {
        cld->Update(dt);
    }
}
