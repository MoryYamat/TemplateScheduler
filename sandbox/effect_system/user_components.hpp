#pragma once

namespace es
{
    struct Input{};
    struct Intent{};

    struct Position{};
    struct Velocity{};
    struct Acceleration{};

    struct Collision{};
    struct CollisionMask{};
    struct CollisionResult{};

    struct Camera{};
    struct Mesh{};

    //test for duplicate in reads/writes detection
    struct DUP_R{};
    struct DUP_W{};

    // test for effects completeness
    struct EC_R{};
    struct EC_W{};
}