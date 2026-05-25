#pragma once

#include <iostream>
#include <mutex>
#include <vector>
#include <chrono>

namespace es
{
    struct AsyncTestContext
    {
        std::mutex mutex;
        std::vector<std::string> log;

        void Push(std::string s)
        {
            std::lock_guard<std::mutex> lock(mutex);
            log.push_back(std::move(s));
        }
    };

    struct Integrate
    {
        static void Run()
        {
            std::cerr << "`Integrate` called\n";
        }
    };
    struct CollisionDetection
    {
        static void Run(AsyncTestContext& ctx)
        {
            std::cerr << "`CollisionDetection` called\n";

            // throw std::runtime_error("collision failed");
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
            ctx.Push("CollisionDetection");
        }
    };
    struct Renderer
    {
        static void Run()
        {
            std::cerr << "`Renderer` called\n";
        }
    };
    struct CameraController
    {
        static void Run(AsyncTestContext& ctx)
        {
            std::cerr << "`CameraController` called\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            ctx.Push("CameraController");
        }
    };
    struct InputSystem
    {
        static void Run()
        {
            std::cerr << "`InputSystem` called\n";
        }
    };
    struct IntentSystem
    {
        static void Run()
        {
            std::cerr << "`IntentSystem` called\n";
        }
    };
}//namespace es