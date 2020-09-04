#include "actor.h"

#include <glm/gtc/matrix_transform.inl>

using namespace tst;

// ���������� �� �������� ������

Actor::~Actor()
{
}

Actor::Actor()
    : _position(0.f, 0.f, 0.f)
    , _angle(0.f)
    , _scale(1.f, 1.f, 1.f)
{
}

// ���������� �� �������� ������
void Actor::AddChild(const std::shared_ptr<Actor>& actor)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _newChildren.push_back(actor);
}

// ���������� �� �������� ������
void Actor::SetPosition2D(float x, float y, bool forcedLock /*= true*/)
{
    SetPosition(glm::vec3(x, y, 0.f));
}

// ���������� �� �������� ������
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

// ���������� �� �������� ������
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

// ������ Render & Update �������� �������� �����, �� �� ���� �������� ������
void Actor::Render(const glm::mat4& m) const
{
    glm::mat4 tmp(m * _local);
    for (auto& cld : _children)
    {
        cld->Render(tmp);
    }
}

// ������ Render & Update �������� �������� �����, �� �� ���� �������� ������
// � �������� ������ ������ �� ������������� std::mutex::lock(),
// ����� �� ������ ����������� ������ ��������� ������,
// �� ��� ��������� � ���������� ����������� ����������,
// � ������ ������ ��� ���� ����� ��������
void Actor::Update(float dt)
{
    TweenPlayer::Update(this, dt);
    if (_mutex.try_lock())
    {
        if (_newChildren.size())
        {
            // get new children
            _children.insert(_children.end(), _newChildren.begin(), _newChildren.end());
            _newChildren.clear();
        }

        // todo: ��� ����� ����������� �������
        // �������������� ���������� ������������, ���� ��� �� ��������� �������(������� �������� ��� ��������, ��������� �����)

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
