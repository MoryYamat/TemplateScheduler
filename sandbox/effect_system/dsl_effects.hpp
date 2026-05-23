#pragma once

#include "tsr/effects/effect.hpp"
#include "user_components.hpp"
#include "user_effects.hpp"

namespace tsr
{
    template<>
    struct Effects<es::Integrate>
    {
        using reads = ResourcePack<es::Velocity, es::Acceleration>;
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
        using reads = ResourcePack<es::Position>;
        using writes = ResourcePack<>;
    };
}// es