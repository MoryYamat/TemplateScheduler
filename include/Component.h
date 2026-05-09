#pragma once
#include <iostream>
#include <cstdint>

namespace tsr
{
    struct Pos
    {
        float x;
        float y;
        float z;

        // test
        static void Update()
        {
            std::cerr << "Pos" << "\n";
        }
    };

    struct Vel
    {
        float x;
        float y;
        float z;
        
        static void Update()
        {
            std::cerr << "Vel" << "\n";
        }
    };

    struct Acc
    {
        float x;
        float y;
        float z;
        
        static void Update()
        {
            std::cerr << "Acc" << "\n";
        }
    };

    struct Dis// distance
    {
        float v;
        
        static void Update()
        {
            std::cerr << "Dis" << "\n";
        }
    };

    struct Collision
    {
        bool coll = false;
        
        static void Update()
        {
            std::cerr << "Collision" << "\n";
        }
    };

    struct CollisionMask
    {
        std::uint32_t mask{};
        
        static void Update()
        {
            std::cerr << "CollisionMask" << "\n";
        }
    };
}