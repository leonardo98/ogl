// Do Not Use It!
// This code is experimental. It can damage you software or hardware.

#pragma once

#ifndef TWEENS_H
#define TWEENS_H

// Tweens - твины анимации движения
// создаются, исключительно, в игровом потоке
// проигрываются, исключительно, в главном потоке


#include <glm/glm.hpp>

#include <list>
#include <memory>
#include <mutex>

#define M_PI 3.141592653f

namespace tst
{

    class Actor;

    class TweenEventListener
    {
    public:
        virtual void TweenEvent() {}
    };

    class Tween
    {
    private:
        TweenEventListener* _listener;
        friend class TweenPlayer;
        virtual void Init(Actor* actor) {}
    public:
        void SetListener(TweenEventListener* l) { _listener = l; }
        const bool queue;
        Tween(bool Queue = false) : queue(Queue), _listener(NULL) {}
        virtual ~Tween() { if (_listener) _listener->TweenEvent(); }
        virtual void UpdatePosition(Actor* actor, float) {}
    };

    // todo: отрефакторить, добавить больше твинов (подсмотреть в Defold)
    enum class MotionType
    {
        Linear,
        Fast,
        JumpOut,
        Slow
    };

    struct PlaySubTween
    {
    private:
        std::shared_ptr<Tween> tween;
        float timeFull;
        float timer;
        MotionType motion;
        bool pingPong;
        int repeat;
        bool reverce;
        void Update(float dt);
        float Timer();
        int GetRepeat() { return repeat; }
        bool GetPingPong() { return pingPong; }
        bool GetReverce() { return reverce; }
        PlaySubTween(const std::shared_ptr<Tween> &t, float time, MotionType motionType)
            : tween(t)
            , timeFull(time)
            , timer(0.f)
            , motion(motionType)
            , repeat(1)
            , reverce(false)
            , pingPong(false)
        {}

        friend class TweenPlayer;
        friend class TweenQueue;
    public:
        PlaySubTween &PingPong(bool b) { pingPong = b; return *this; }
        PlaySubTween &Repeat(int r) { repeat = r; return *this; }
        PlaySubTween& SetReverce(bool b) { reverce = b; return *this; }
        PlaySubTween& SetMotion(MotionType motionType) { motion = motionType; return *this; }
    };

    typedef std::list<PlaySubTween> TweenList;

    // todo: нужно предусмотреть, чтобы очередь твинов не начала выполняться, пока ее заполняем
    class TweenQueue : public Tween
    {
    private:
        TweenList _queue;
    public:
        TweenQueue() : Tween(true) {}
        std::shared_ptr<Tween> Add(std::shared_ptr<Tween>& t, float time, MotionType motionType = MotionType::Linear);
        void Clear();
        size_t Size();
        virtual void UpdatePosition(Actor* actor, float dt);
    };

    //class TweenAlpha : public Tween
    //{
    //private:
    //    float _alpha;
    //    float _alphaStart;
    //public:
    //    TweenAlpha(float alpha)
    //        : _alpha(alpha)
    //        , _alphaStart(-1)
    //    {}
    //    virtual void UpdatePosition(Actor *actor, float p);
    //};

    //class TweenColor : public Tween
    //{
    //private:
    //    float _r;
    //    float _g;
    //    float _b;
    //    float _a;
    //    float _rStart;
    //    float _gStart;
    //    float _bStart;
    //    float _aStart;
    //public:
    //    TweenColor(unsigned int color)
    //        : _r(static_cast<float>((color & 0xFF0000) >> 16))
    //        , _g(static_cast<float>((color & 0xFF00) >> 8))
    //        , _b(static_cast<float>((color & 0xFF)))
    //        , _a(static_cast<float>((color & 0xFF000000) >> 24))
    //        , _rStart(-1.f)
    //    {}
    //    virtual void UpdatePosition(Actor *actor, float p);
    //};

    class TweenEmpty : public Tween
    {
    public:
        TweenEmpty() {}
        virtual void UpdatePosition(Actor* actor, float p) {}
    };

    class TweenPosition : public Tween
    {
    private:
        glm::vec3 _pos;
        glm::vec3 _posStart;
    public:
        TweenPosition(const glm::vec3& pos)
            : _pos(pos)
        {}
        void Init(Actor* actor) override;
        void UpdatePosition(Actor* actor, float p) override;
    };

    class TweenRotate2D : public Tween
    {
    private:
        float _pos;
        float _posStart;
    public:
        TweenRotate2D(float pos)
            : _pos(pos)
            , _posStart(pos)
        {}
        void Init(Actor* actor) override;
        void UpdatePosition(Actor* actor, float p) override;
    };

    //class TweenTimedCommand : public Tween
    //{
    //private:
    //    std::string _cmd;
    //public:
    //    TweenTimedCommand(const std::string &cmd)
    //        : _cmd(cmd)
    //    {}
    //    virtual void UpdatePosition(Actor *actor, float p);
    //};

    class TweenScale : public Tween
    {
    private:
        glm::vec3 _scale;
        glm::vec3 _scaleStart;
    public:
        TweenScale(const glm::vec3& scale)
            : _scale(scale)
            , _scaleStart(scale)
        {}
        TweenScale(float scale)
            : _scale(scale)
        {}
        void Init(Actor* actor) override;
        void UpdatePosition(Actor* actor, float p) override;
    };

    class TweenPlayer
    {
    private:
        TweenList _tweens;
        TweenList _tweensIncoming;
        std::mutex _mutex;
    public:
        // методы вызываются из игрового потока
        template <typename T, typename... Targs>
        PlaySubTween &CreateTween(float time, Targs... Fargs)
        {
            std::shared_ptr<T> a;
            a.reset(new T(Fargs...));
            return AddTween(a, time, MotionType::Linear);
        }

        TweenPlayer() {}

        PlaySubTween &AddTween(const std::shared_ptr<Tween> &t, float time = 0, MotionType motionType = MotionType::Linear);

        // методы вызываются из главного потока
        virtual ~TweenPlayer() { ClearTweens(); }
        bool Update(Actor* actor, float dt);
        void ClearTweens();
        void TweenCheck();
        bool Action() { return _tweens.size() > 0; }
    };

}

#endif//TWEENS_H
