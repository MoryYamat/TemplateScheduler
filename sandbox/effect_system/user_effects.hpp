#pragma once

#include <iostream>
#include <mutex>
#include <vector>
#include <chrono>
#include <thread>
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
            std::cerr << "`Integrate` called on " << std::this_thread::get_id() << "\n";
        }
    };
    struct CollisionDetection
    {
        static void Run(AsyncTestContext& ctx)
        {
            std::cerr << "`CollisionDetection` called on " << std::this_thread::get_id() << "\n";

            // throw std::runtime_error("collision failed");
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
            ctx.Push("CollisionDetection");
        }
    };
    struct Renderer
    {
        static void Run()
        {
            std::cerr << "`Renderer` called on " << std::this_thread::get_id() << "\n";
        }
    };
    struct CameraController
    {
        static void Run(AsyncTestContext& ctx)
        {
            std::cerr << "`CameraController` called on " << std::this_thread::get_id() << "\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            ctx.Push("CameraController");
        }
    };
    struct InputSystem
    {
        static void Run()
        {
            std::cerr << "`InputSystem` called on " << std::this_thread::get_id() << "\n";
        }
    };
    struct IntentSystem
    {
        static void Run()
        {
            std::cerr << "`IntentSystem` called on " << std::this_thread::get_id() << "\n";
        }
    };

    
}//namespace es