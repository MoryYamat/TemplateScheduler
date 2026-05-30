#pragma once

#include "user_components.hpp"
#include "user_effects.hpp"
#include "tsr/effects/effect.hpp"
#include "tsr/analysis/costs.hpp"

namespace tsr
{
    template<>
    struct Effects<es::Integrate>
    {
        using reads = ResourcePack<es::Intent,es::Velocity, es::Acceleration>;
        using writes = ResourcePack<es::Position>;
    };

    template<>
    struct Effects<es::CollisionDetection>
    {
        using reads = ResourcePack<es::Position, es::Collision, es::CollisionMask>;
        using writes = ResourcePack<es::CollisionResult>;
    };

    template<>
    struct Effects<es::Renderer>
    {
        using reads = ResourcePack<es::Camera, es::Position>;
        using writes = ResourcePack<>;
    };

    template<>
    struct Effects<es::IntentSystem>
    {
        using reads = ResourcePack<es::Input>;
        using writes = ResourcePack<es::Intent>;
    };

    template<>
    struct Effects<es::CameraController>
    {
        using reads = ResourcePack<es::Position>;
        using writes = ResourcePack<es::Camera>;
    };

    // **** cost **** 
        template<>
    struct DeclaredCost<es::Integrate>
    {
        static constexpr std::size_t value = 1;
    };

    template<>
    struct DeclaredCost<es::CollisionDetection>
    {
        static constexpr std::size_t value = 2;
    };

    template<>
    struct DeclaredCost<es::Renderer>
    {
        static constexpr std::size_t value = 8;
    };

    template<>
    struct DeclaredCost<es::IntentSystem>
    {
        static constexpr std::size_t value = 2;
    };

    template<>
    struct DeclaredCost<es::CameraController>
    {
        static constexpr std::size_t value = 2;
    };

    // test for duplicate in reads/writes detection
    template<>
    struct Effects<es::DUP_R>
    {
        using reads = ResourcePack<es::Position, es::Collision, es::Position>;
        using writes = ResourcePack<>;
    };
    template<>
    struct Effects<es::DUP_W>
    {
        using reads = ResourcePack<>;
        using writes = ResourcePack<es::Position, es::Collision, es::Position>;
    };

    // test for effects completeness
    template<>
    struct Effects<es::EC_R>
    {
        using writes = ResourcePack<es::Position>;
    };
    template<>
    struct Effects<es::EC_W>
    {
        using reads = ResourcePack<es::Position>;
    };
}// es