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
    
    using  N_Root =         Node<Root>;
    using  N_CLeft_1 =      Node<CLeft_1>;
    using  N_CRight_1 =     Node<CRight_1>;
    using  N_CRight_2 =     Node<CRight_2>;
    using  N_CCLeft_1 =     Node<CCLeft_1>;
    using  N_CCLeft_2 =     Node<CCLeft_2>;
    using  N_CCRight_1 =    Node<CCRight_1>;
    using  N_CCCLeft_1 =    Node<CCCLeft_1>;
    using  N_CCCRight_1 =   Node<CCCRight_1>;

    using A_CCRight = Arc<N_CCRight_1, N_CCCLeft_1, N_CCCRight_1>;
    using A_CRight = Arc<N_CRight_1, N_CCLeft_1, N_CCLeft_2, A_CCRight>;
    using A_Root = Arc<N_Root, N_CLeft_1, A_CRight, N_CRight_2>;
    struct TestTag{};
    // using TestGraph = Graph<TestTag, A_Root;
    using TestGraph = Graph<TestTag, A_Root, A_Root>;// need to be errored

    // Test build with another method

    // CYCLIC TEST
    using Cy_N_1 = Node<E_1>;
    using Cy_N_2 = Node<E_2>;
    using Cy_N_3 = Node<E_3>;
    using Cy_N_4 = Node<E_4>;

    using Cy_Arc4 = Arc<Cy_N_4, Cy_N_1>;
    using Cy_Arc3 = Arc<Cy_N_3, Cy_Arc4>;
    using Cy_Arc2 = Arc<Cy_N_2, Cy_Arc3>;
    using Cy_Arc1 = Arc<Cy_N_1, Cy_Arc2>;

    struct TestCylicTag{};
    using Cy_Graph = Graph<TestCylicTag, Cy_Arc1>;
}