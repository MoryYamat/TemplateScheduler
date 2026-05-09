#pragma once

#include "Component.h"
#include "HierarchyConstructor.h"

namespace tsr
{
    using PosNode = Node<Pos>;
    using DisNode = Node<Dis>;// distance
    using VelNode = Node<Vel>;
    using AccNode = Node<Acc>;

    using Ontology_Vel_Depend = Arc<VelNode, AccNode>;
    using Ontology_Pos_Depend = Arc<PosNode, DisNode, Ontology_Vel_Depend>;  // Graph Root

    using TaskDepndency_Dis_Depend = Arc<PosNode, DisNode>;
    using TaskDepndency_Vel_Depend = Arc<VelNode, PosNode>;
    using TaskDepndency_Acc_Depend = Arc<AccNode, TaskDepndency_Vel_Depend>;     // Graph Root

    struct PhysTag{};
    using Ontology_PysGraph = Graph<PhysTag, Ontology_Pos_Depend>;

    using TaskDepndency_PysGraph = Graph<PhysTag, TaskDepndency_Acc_Depend, TaskDepndency_Dis_Depend>;
}