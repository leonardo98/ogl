#include "scene/material.h"
#include "scene/texture.h"
#include "scene/sprite.h"
#include "scene/mouse_area.h"

#include "core/signal.h"
#include "system/input_system.h"
#include "system/actors/debug_render_actor.h"

//#include <glm/gtc/constants.inl>
#include <glm/gtc/matrix_transform.hpp>

#include <thread>
#include <chrono>

tst::Signal WaitForSignal(tst::SignalType signalType, float time = 0.f)// wait for input while time in sec
{
    while (true)
    {
        //if (!tst::InputSystem::Instance()->IsEmpty())
        //{
        //    tst::Signal s = tst::InputSystem::Instance()->PopSignal();
        //    if (s.signalType == signalType)
        //    {
        //        return s;
        //    }
        //}
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(10ms);
        if (time > 0.f)
        {
            time -= 0.01f;
            if (time <= 0.f)
            {
                break;
            }
        }
    }
    return tst::Signal();
}

void game(tst::Actor * root)
{
    // load material
    // sp_material - getting shared pointer to material
    auto sp_material = root->Add<tst::Material>("TransformVertexShader.vertexshader", "TextureFragmentShader.fragmentshader");
    {
        // 2D case
        glm::mat4x4 projectionMatrix = glm::ortho(0.f, 1024.f, 0.f, 768.f);
        glm::mat4 viewMatrix = glm::mat4(1.f);
        viewMatrix = glm::translate(viewMatrix, glm::vec3(0.f, 768.f, 0.f));
        viewMatrix = glm::scale(viewMatrix, glm::vec3(1.f, -1.f, 1.f));

        sp_material->SetMatrix(projectionMatrix * viewMatrix);
    }

    auto debugRender = root->Add<tst::DebugRenderActor>();
    {
        // 2D case
        glm::mat4x4 projectionMatrix = glm::ortho(0.f, 1024.f, 0.f, 768.f);
        glm::mat4 viewMatrix = glm::mat4(1.f);
        viewMatrix = glm::translate(viewMatrix, glm::vec3(0.f, 768.f, 0.f));
        viewMatrix = glm::scale(viewMatrix, glm::vec3(1.f, -1.f, 1.f));

        debugRender->SetMatrix(projectionMatrix * viewMatrix);
    }

    // make a simple loading screen with animation
    // load texture
    // sp_texture - shared pointer to texture
    auto sp_texture = sp_material->Add<tst::Texture>(sp_material, "circle.png");

    // make a tree of sprites
    // circleRoot - shared pointer to sprite
    tst::SharedActor circleRoot = sp_texture->Add<tst::Actor>();
    {
        circleRoot->SetPosition2D(1024.f / 2, 768.f / 2);
        circleRoot->SetScale(glm::vec3(0.f));
    }

    // circle1 & circle2 - shared pointers to sprites
    tst::SharedActor circle1 = circleRoot->Add<tst::Sprite>(sp_texture, 0, 0, sp_texture->Width(), sp_texture->Height());
    tst::SharedActor circle2 = circleRoot->Add<tst::Sprite>(sp_texture, 0, 0, sp_texture->Width(), sp_texture->Height());
    {
        circle1->SetPosition2D(64.f, 0.f);
        circle2->SetPosition2D(-64.f, 0.f);
    }

    circleRoot->Add<tst::MouseArea>(200, 50);

    // add animations
    circleRoot->CreateTween<tst::TweenRotate2D>(1.5f, 2 * glm::pi<float>()).Repeat(0);
    circleRoot->CreateTween<tst::TweenScale>(1.4f, 1.f).SetMotion(tst::MotionType::JumpOut);

    const tst::Signal s = WaitForSignal(tst::SignalType::MouseDown, 3.f);

    for (int i = 0; i < 100; ++i)
    {
        // circle - shared pointer to sprite
        tst::SharedActor circle = sp_texture->Add<tst::Sprite>(sp_texture, 0, 0, sp_texture->Width(), sp_texture->Height());

        // set sprite position on screen
        circle->SetPosition2D(rand() % (1024 - 32 * 3) + 32, rand() % (768 - 32 * 3) + 32);

        // set sprite scale on screen
        circle->SetScale2D(0.f, 0.f);

        // add animations
        circle->CreateTween<tst::TweenScale>(0.3f, 1.f).SetMotion(tst::MotionType::JumpOut);
        //{
        //    // waiting for 100 ms
        //    using namespace std::chrono_literals;
        //    std::this_thread::sleep_for(100ms);
        //}

        const tst::Signal s = WaitForSignal(tst::SignalType::MouseDown, 0.1f);
        if (s.signalType == tst::SignalType::MouseDown)
        {
            break;
        }
    }
    
    WaitForSignal(tst::SignalType::MouseDown, 3.f);

    // exit app
}
