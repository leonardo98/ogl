#pragma once

#ifndef ACTOR_STATE_H
#define ACTOR_STATE_H

namespace tst
{
    enum class ActorState
    {
        None,
        Failed,
        Loaded,
        Binded
    };
}

#endif//ACTOR_STATE_H