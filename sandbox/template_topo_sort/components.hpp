#pragma once
#include <cstdint>

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

    struct Dis// distance
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
    };
   
    //TEST
    struct Root
    {
        using type = Root;
    };
    struct CLeft_1
    {
        using type = CLeft_1;
    };
    struct CRight_1
    {
        using type = CRight_1;
    };
    struct CRight_2
    {
        using type = CRight_2;
    };
    struct CCLeft_1
    {
        using type = CCLeft_1;
    };
    struct CCLeft_2
    {
        using type = CCLeft_2;
    };
    struct CCRight_1
    {
        using type = CCRight_1;
    };
    struct CCCLeft_1
    {
        using type = CCCLeft_1;
    };
    struct CCCRight_1
    {
        using type = CCCRight_1;
    };

    // CYCLIC TEST
    struct E_1{};
    struct E_2{};
    struct E_3{};
    struct E_4{};
}