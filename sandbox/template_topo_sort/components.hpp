#pragma once
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

namespace tsr
{
    struct Pos
    {
        float x;
        float y;
        float z;
    };

    struct Vel
    {
        float x;
        float y;
        float z;
    };

    struct Acc
    {
        float x;
        float y;
        float z;
    };

    struct Dis // distance
    {
        float v;
    };

    struct Collision
    {
        bool coll = false;
    };

    struct CollisionMask
    {
        std::uint32_t mask{};

        static void Run()
        {
            std::cerr << "CollisionMask\n";
        }
    };

    //TEST
    struct Root
    {
    };
    struct CLeft_1
    {
    };
    struct CRight_1
    {
    };
    struct CRight_2
    {
    };
    struct CCLeft_1
    {
    };
    struct CCLeft_2
    {
    };
    struct CCRight_1
    {
    };
    struct CCCLeft_1
    {
    };
    struct CCCRight_1
    {
    };

    // CYCLIC TEST
    struct E_1
    {
    };
    struct E_2
    {
    };
    struct E_3
    {
    };
    struct E_4
    {
    };

    // for testing graph of graphs
    // graph A
    struct GG_A_R{};
    struct GG_A_0{};
    struct GG_A_1{};
    struct GG_A_2{};
    struct GG_A_3{};

    // graph B
    struct GG_B_R{};
    struct GG_B_0{};
    struct GG_B_1{};
    struct GG_B_2{};
    struct GG_B_3{};

    // graph C
        // graph B
    struct GG_C_R{};
    struct GG_C_0{};
    struct GG_C_1{};
    struct GG_C_2{};
    struct GG_C_3{};


    
    struct TestContext
    {
        std::vector<std::string> log;
    };

    struct A{};
    struct B{};
    struct C
    {
        static void Run(TestContext& ctx)
        {
            ctx.log.push_back("static_context");
        }
    };
    struct D
    {
        inline static bool called = false;

        static void Run()
        {
            called = true;
        }
    };
} // namespace tsr