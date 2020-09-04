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

        // методы вызываются из игрового потока
        template <typename T, typename... Targs>
        std::shared_ptr<T> Add(Targs... Fargs)
        {
            std::shared_ptr<T> a;
            a.reset(new T(Fargs...));
            AddChild(a);
            return a;
        }

        // методы вызываются из игрового потока
        void AddChild(const std::shared_ptr<Actor>& actor);
        Actor();

        // параметры описывающие положение актера
        // методы вызываются из игрового потока
        void SetPosition2D(float x, float y, bool forcedLock = true);
        void SetScale2D(float sx, float sy, bool forcedLock = true);

        void SetPosition(const glm::vec3 &pos, bool forcedLock = true);
        void SetScale(const glm::vec3& scale, bool forcedLock = true);

        const glm::vec3 GetPosition() const;
        const glm::vec3 GetScale() const;

        void SetRotation2D(float angle, bool forcedLock = true);
        float GetRotation2D() const;

    protected:

        // методы Render & Update вызывает основной поток, их не надо вызывать самому
        virtual void Render(const glm::mat4& m) const;
        virtual void Update(float dt);

        // к этим полям обращение может быть только из главного потока
        typedef std::list<std::shared_ptr<Actor>> Actors;
        Actors _children;
        glm::mat4 _local;

    private:
        // к этим полям обращаемся из любого потока
        Actors _newChildren;
        glm::vec3 _position;
        float _angle;
        glm::vec3 _scale;
        mutable std::mutex _mutex;

        // только main(основной поток) может вызывать Render & Update
        friend int ::main(void);
    };
    typedef std::shared_ptr<Actor> SharedActor;
}

#endif//TST_ACTOR_H