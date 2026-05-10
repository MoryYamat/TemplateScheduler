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
   
    //TEST
    struct Root{};
    struct CLeft_1{};
    struct CRight_1{};
    struct CRight_2{};
    struct CCLeft_1{};
    struct CCLeft_2{};
    struct CCRight_1{};
    struct CCCLeft_1{};
    struct CCCRight_1{};

    // CYCLIC TEST
    struct E_1{};
    struct E_2{};
    struct E_3{};
    struct E_4{};
}