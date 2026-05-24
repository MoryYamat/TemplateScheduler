#pragma once

#include <iostream>

namespace es
{
    struct Integrate
    {
        static void Run()
        {
            std::cerr << "`Integrate` called\n";
        }
    };
    struct CollisionDetection
    {
        static void Run()
        {
            std::cerr << "`CollisionDetection` called\n";
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
        static void Run()
        {
            std::cerr << "`CameraController` called\n";
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