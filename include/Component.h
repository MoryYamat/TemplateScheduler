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
    struct Root
    {
        using type = Root;
        static void Run()
        {
            std::cerr << "Root = " << typeid(type).name() << "\n";
        };
    };
    struct CLeft_1
    {
        using type = CLeft_1;
        static void Run()
        {
            std::cerr << "Root = " << typeid(type).name() << "\n";
        };
    };
    struct CRight_1
    {
        using type = CRight_1;
        static void Run()
        {
            std::cerr << "Root = " << typeid(type).name() << "\n";
        };
    };
    struct CRight_2
    {
        using type = CRight_2;
        static void Run()
        {
            std::cerr << "Root = " << typeid(type).name() << "\n";
        };
    };
    struct CCLeft_1
    {
        using type = CCLeft_1;
        static void Run()
        {
            std::cerr << "Root = " << typeid(type).name() << "\n";
        };
    };
    struct CCLeft_2
    {
        using type = CCLeft_2;
        static void Run()
        {
            std::cerr << "Root = " << typeid(type).name() << "\n";
        };
    };
    struct CCRight_1
    {
        using type = CCRight_1;
        static void Run()
        {
            std::cerr << "Root = " << typeid(type).name() << "\n";
        };
    };
    struct CCCLeft_1
    {
        using type = CCCLeft_1;
        static void Run()
        {
           std::cerr << "Root = " << typeid(type).name() << "\n";
        };
    };
    struct CCCRight_1
    {
        using type = CCCRight_1;
        static void Run()
        {
            std::cerr << "Root = " << typeid(type).name() << "\n";
        };
    };

    // CYCLIC TEST
    struct E_1{};
    struct E_2{};
    struct E_3{};
    struct E_4{};
}