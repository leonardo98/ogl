#include "tweens.h"
#include "actor.h"

#include <assert.h>

using namespace tst;

template <class T>
T lerp(const T &one, const T &two, float p) {
    return (two - one) * p + one;
}

void PlaySubTween::Update(float dt)
{
    timer = timer + dt / timeFull;
}

float PlaySubTween::Timer()
{
    if (motion == MotionType::Linear)
    {
        return timer;
    }
    if (motion == MotionType::Slow)
    {
        return sin(M_PI / 2 * timer);
    }
    if (motion == MotionType::Fast)
    {
        return 1 - cos(M_PI / 2 * timer);
    }
    if (motion == MotionType::JumpOut)
    {
        if (timer < 0.5f)
            return 1.15f * sin(M_PI * timer);
        else
            return 1.f + 0.15f * sin(M_PI * timer);
    }
    assert(false);
    return -1.f;
}

std::shared_ptr<Tween>TweenQueue::Add(std::shared_ptr<Tween>& t, float time, MotionType motionType)
{
    assert(t != NULL && !(t->queue));
    if (time <= 0)
    {
        return NULL;
    }
    _queue.push_back(PlaySubTween(t, time, motionType)); 
    return t;
}

void TweenQueue::Clear()
{ 
    _queue.clear();
}

size_t TweenQueue::Size() { return _queue.size(); }

void TweenQueue::UpdatePosition(Actor *actor, float dt)
{
    if (_queue.size() == 0)
    {
        return;
    }
    PlaySubTween &play = _queue.front();
    play.Update( dt );
    if (play.timer < 1.f)
    {
        play.tween->UpdatePosition(actor, play.Timer());
    }
    else
    {
        float overHead = (play.timer - 1.f) * play.timeFull;
        play.tween->UpdatePosition(actor, 1.f);
        _queue.pop_front();
        if (overHead > 0)
        {
            UpdatePosition(actor, overHead);
        }
    }
}

//void TweenAlpha::UpdatePosition(Actor *actor, float p) 
//{
//    if (_alphaStart < 0)
//    {
//        _alphaStart = actor->GetAlpha();
//    }
//    actor->SetAlpha(lerp(_alphaStart, _alpha, p));
//}
//
//void TweenColor::UpdatePosition(Actor *actor, float p) 
//{
//    if (_rStart < 0)
//    {
//        _rStart = static_cast<float>((actor->GetColor() & 0xFF0000) >> 16);
//        _gStart = static_cast<float>((actor->GetColor() & 0xFF00) >> 8);
//        _bStart = static_cast<float>((actor->GetColor() & 0xFF));
//        _aStart = static_cast<float>((actor->GetColor() & 0xFF000000) >> 24);
//    }
//    actor->SetColor(static_cast<unsigned int>(lerp(_rStart, _r, p)) << 16 
//                    | static_cast<unsigned int>(lerp(_gStart, _g, p)) << 8 
//                    | static_cast<unsigned int>(lerp(_bStart, _b, p)) 
//                    | static_cast<unsigned int>(lerp(_aStart, _a, p)) << 24 );
//}

void TweenPosition::Init(Actor* actor)
{
    _posStart = actor->GetPosition();
}

void TweenPosition::UpdatePosition(Actor *actor, float p) 
{
    actor->SetPosition(lerp(_posStart, _pos, p));
}

void TweenRotate2D::Init(Actor* actor)
{
    _posStart = actor->GetRotation2D();
}

void TweenRotate2D::UpdatePosition(Actor *actor, float p) 
{
    actor->SetRotation2D(lerp(_posStart, _pos, p));
}

void TweenScale::Init(Actor* actor)
{
    _scaleStart = actor->GetScale();
}

void TweenScale::UpdatePosition(Actor *actor, float p) 
{
    glm::vec3 s(lerp(_scaleStart, _scale, p));
    actor->SetScale(s);
}

//void TweenTimedCommand::UpdatePosition(Actor *actor, float p)
//{
//    if (p == 1 && _cmd.size())
//    {
//        actor->Command(_cmd);
//        _cmd.clear();
//    }
//}

PlaySubTween &TweenPlayer::AddTween(const std::shared_ptr<Tween> &t, float time, MotionType motionType)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _tweensIncoming.push_back(PlaySubTween(t, time, motionType));
    if (auto hack = dynamic_cast<Actor*>(this))
    {
        _tweensIncoming.back().tween->Init(hack);
    }
    return _tweensIncoming.back();
}

void TweenPlayer::Update(Actor *actor, float dt)
{
    if (dt > 0.1f)
    {
        dt = 0.1f;
    }
    if (_mutex.try_lock())
    {
        if (_tweensIncoming.size())
        {
            // get new children
            _tweens.insert(_tweens.end(), _tweensIncoming.begin(), _tweensIncoming.end());
            _tweensIncoming.clear();
        }
        _mutex.unlock();
    }

    for (TweenList::iterator i = _tweens.begin(); i != _tweens.end(); )
    {
        PlaySubTween &play = *i;
        if (play.tween->queue)
        {
            TweenQueue *queue = dynamic_cast<TweenQueue *>(play.tween.get());
            if (queue == NULL || queue->Size() == 0)
            {
                i = _tweens.erase(i);
            }
            else
            {
                play.tween->UpdatePosition(actor, dt);
                ++i;
            }
        }
        else
        {
            play.Update( dt );
            
            if (play.timer >= 1.f) {
                if (play.GetRepeat() == 0) {
                    play.timer -= 1.f;
                    if (play.GetPingPong()) {
                        play.SetReverce(!play.GetReverce());
                    }
                } else if (play.GetPingPong() && !play.GetReverce()) {
                    play.SetReverce(true);
                    play.timer -= 1.f;
                } else if (play.GetPingPong() && play.GetReverce() && play.GetRepeat() > 1) {
                    play.Repeat(play.GetRepeat() - 1);
                    play.SetReverce(false);
                    play.timer -= 1.f;
                }
            }

            if (play.timer < 1.f)
            {
                if (play.GetReverce()) {
                    play.tween->UpdatePosition(actor, 1.f - play.Timer());
                }
                else {
                    play.tween->UpdatePosition(actor, play.Timer());
                }
                ++i;
            }
            else
            {
                play.tween->UpdatePosition(actor, 1);
                i = _tweens.erase(i);
            }
        }

    }
}

void TweenPlayer::ClearTweens()
{
    _tweens.clear();
}

void TweenPlayer::TweenCheck()
{
    assert(_tweens.size() == 0);
}
