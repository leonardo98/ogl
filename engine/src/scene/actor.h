#pragma once

#ifndef TST_ACTOR_H
#define TST_ACTOR_H

#include "scene/tweens.h"

#include <glm/glm.hpp>

#include <list>
#include <memory>
#include <mutex>

extern int main(void);

namespace tst
{
    class Actor : public TweenPlayer
    {
    public:
        virtual ~Actor();

        // ������ ���������� �� �������� ������
        template <typename T, typename... Targs>
        std::shared_ptr<T> Add(Targs... Fargs)
        {
            std::shared_ptr<T> a;
            a.reset(new T(Fargs...));
            AddChild(a);
            return a;
        }

        // ������ ���������� �� �������� ������
        void AddChild(const std::shared_ptr<Actor>& actor);
        Actor();

        // ��������� ����������� ��������� ������
        // ������ ���������� �� �������� ������
        void SetPosition2D(float x, float y, bool forcedLock = true);
        void SetScale2D(float sx, float sy, bool forcedLock = true);

        void SetPosition(const glm::vec3 &pos, bool forcedLock = true);
        void SetScale(const glm::vec3& scale, bool forcedLock = true);

        const glm::vec3 GetPosition() const;
        const glm::vec3 GetScale() const;

        void SetRotation2D(float angle, bool forcedLock = true);
        float GetRotation2D() const;

    protected:

        // ������ Render & Update �������� �������� �����, �� �� ���� �������� ������
        virtual void Render(const glm::mat4& m) const;
        virtual void Update(float dt);

        // � ���� ����� ��������� ����� ���� ������ �� �������� ������
        typedef std::list<std::shared_ptr<Actor>> Actors;
        Actors _children;
        glm::mat4 _local;

    private:
        // � ���� ����� ���������� �� ������ ������
        Actors _newChildren;
        glm::vec3 _position;
        float _angle;
        glm::vec3 _scale;
        mutable std::mutex _mutex;

        // ������ main(�������� �����) ����� �������� Render & Update
        friend int ::main(void);
    };
    typedef std::shared_ptr<Actor> SharedActor;
}

#endif//TST_ACTOR_H