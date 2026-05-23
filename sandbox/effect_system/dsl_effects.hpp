#pragma once

#include "tsr/effects/effect.hpp"
#include "user_components.hpp"
#include "user_effects.hpp"

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
}// es