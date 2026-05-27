#pragma once

#include "user_components.hpp"
#include "user_effects.hpp"

#include "tsr/graph/graph_dsl.hpp"

namespace tsr
{
    // test for creating effect system 
    using CES_N_POS = Node<es::Position>;
    using CES_N_VEL = Node<es::Velocity>;
    using CES_N_ACC = Node<es::Acceleration>;
    using CES_N_COLL = Node<es::Collision>;
    using CES_N_MASK = Node<es::CollisionMask>;

    using CES_PROC_N_INTEG = Node<es::Integrate>;
    using CES_PROC_N_COLL = Node<es::CollisionDetection>;
    using CES_PROC_N_RENDER = Node<es::Renderer>;
    using CES_PROC_N_CAMERA = Node<es::CameraController>;
    // using CES_PROC_N_INPUT =Node<es::InputSystem>;
    using CES_PROC_N_INTENT = Node<es::IntentSystem>;

    // test for duplicate in reads/writes detection
    using DP_PROC_N_R = Node<es::DUP_R>;
    using DP_PROC_N_W = Node<es::DUP_W>;

    // test for effects completeness
    using EC_PROC_N_R = Node<es::EC_R>;
    using EC_PROC_N_W = Node<es::EC_W>;
}//namespace tsr