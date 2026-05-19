#pragma once

#include "components.hpp"
#include "tsr/graph/graph_dsl.hpp"

namespace tsr
{
    using N_Root = Node<Root>;
    using N_CLeft_1 = Node<CLeft_1>;
    using N_CRight_1 = Node<CRight_1>;
    using N_CRight_2 = Node<CRight_2>;
    using N_CCLeft_1 = Node<CCLeft_1>;
    using N_CCLeft_2 = Node<CCLeft_2>;
    using N_CCRight_1 = Node<CCRight_1>;
    using N_CCCLeft_1 = Node<CCCLeft_1>;
    using N_CCCRight_1 = Node<CCCRight_1>;

    using A_CCRight = Arc<N_CCRight_1, N_CCCLeft_1, N_CCCRight_1>; // OK
    // using A_CCRight_Error = Arc<N_CCRight_1, N_CCCLeft_1, N_CCCRight_1, N_CCCRight_1>;// need to be errored(simple duplication) :: done
    // using A_CCRithg_Error_Dup = Arc<N_CCRight_1, N_CCCRight_1, N_CCCLeft_1>;// need to be errored(different order duplication)  :: done
    using A_CRight = Arc<N_CRight_1, N_CCLeft_1, N_CCLeft_2, A_CCRight>;
    using A_Root = Arc<N_Root, N_CLeft_1, A_CRight, N_CRight_2>;
    struct TestTag
    {
    };
    using TestGraph = Graph<TestTag, A_Root>;
    // using TestGraph = Graph<TestTag, A_Root, A_Root>;// need to be errored

    // error tester
    // +++++++++++ simple bidirection(done) +++++++++++
    using A_CCRight_Error_Bidirection_1 =
        Arc<N_CCRight_1, N_CCCLeft_1, N_CCCRight_1>; // need to be errored (simple bidirection)
    using A_CCRight_Error_Bidirection_2 =
        Arc<N_CCCLeft_1, N_CCRight_1, N_CCCRight_1>; // need to be errored (simple bidirection)
    using BidirectionalGraph = Graph<TestTag, A_CCRight_Error_Bidirection_1, A_CCRight_Error_Bidirection_2>;
    // using ForceCheck_Simple_Bidirection = typename BidirectionalGraph::relations;

    // // ======  TOPOLOGICALSORT ======
    // struct TP_Root{};
    // struct TP_0{};
    // struct TP_1{};
    // struct TP_2{};
    // struct TP_3{};
    // struct TP_4{};
    // struct TP_5{};

    // using TP_NR = Node<TP_Root>;
    // using TP_N0 = Node<TP_0>;
    // using TP_N1 = Node<TP_1>;
    // using TP_N2 = Node<TP_2>;
    // using TP_N3 = Node<TP_3>;
    // using TP_N4 = Node<TP_4>;
    // using TP_N5 = Node<TP_5>;

    // // test
    // using TP_Arc_0 = Arc<TP_N2, TP_N1>;
    // using TP_Arc_1 = Arc<TP_N0, TP_Arc_0>;
    // using TP_Arc_2 = Arc<TP_NR, TP_Arc_1>;
    // struct TPGraphTag{};
    // using TP_Graph = Graph<TPGraphTag, TP_Arc_2>;

    // CYCLIC TEST
    using Cy_N_1 = Node<E_1>;
    using Cy_N_2 = Node<E_2>;
    using Cy_N_3 = Node<E_3>;
    using Cy_N_4 = Node<E_4>;

    using Cy_Arc4 = Arc<Cy_N_4, Cy_N_1>;
    using Cy_Arc3 = Arc<Cy_N_3, Cy_Arc4>;
    using Cy_Arc2 = Arc<Cy_N_2, Cy_Arc3>;
    using Cy_Arc1 = Arc<Cy_N_1, Cy_Arc2>;

    struct TestCylicTag
    {
    };
    using Cy_Graph = Graph<TestCylicTag, Cy_Arc1>;

    // testing for graph of graphs
    // Graph A
    using GG_A_NR = Node<GG_A_R>;
    using GG_A_N0 = Node<GG_A_0>;
    using GG_A_N1 = Node<GG_A_1>;
    using GG_A_N2 = Node<GG_A_2>;
    using GG_A_N3 = Node<GG_A_3>;
    using GG_A_Arc_1 = Arc<GG_A_N1, GG_A_N2, GG_A_N3>;
    using GG_A_Arc_0 = Arc<GG_A_NR, GG_A_N0, GG_A_Arc_1>;
    struct GG_TEST_A_GraphTag
    {
    };
    using GG_A_Graph = Graph<GG_TEST_A_GraphTag, GG_A_Arc_0>;

    // Graph B
    using GG_B_NR = Node<GG_B_R>;
    using GG_B_N0 = Node<GG_B_0>;
    using GG_B_N1 = Node<GG_B_1>;
    using GG_B_N2 = Node<GG_B_2>;
    using GG_B_N3 = Node<GG_B_3>;
    using GG_B_Arc_1 = Arc<GG_B_N1, GG_B_N2, GG_B_N3>;
    using GG_B_Arc_0 = Arc<GG_B_NR, GG_B_N0, GG_B_Arc_1>;
    struct GG_TEST_B_GraphTag
    {
    };
    using GG_B_Graph = Graph<GG_TEST_B_GraphTag, GG_B_Arc_0>;

    // Graph C
    using GG_C_NR = Node<GG_C_R>;
    using GG_C_N0 = Node<GG_C_0>;
    using GG_C_N1 = Node<GG_C_1>;
    using GG_C_N2 = Node<GG_C_2>;
    using GG_C_N3 = Node<GG_C_3>;
    using GG_C_Arc_1 = Arc<GG_C_N1, GG_C_N2, GG_C_N3>;
    using GG_C_Arc_0 = Arc<GG_C_NR, GG_C_N0, GG_C_Arc_1>;
    struct GG_TEST_C_GraphTag
    {
    };
    using GG_C_Graph = Graph<GG_TEST_C_GraphTag, GG_C_Arc_0>;


    // =================== TEST FOR EXECUTE META GRAPH (PLAN) =================== 
    // Positions
    using PosNode = Node<Pos>;
    using DisNode = Node<Dis>; // distance
    using VelNode = Node<Vel>;
    using AccNode = Node<Acc>;

    using Ontology_Vel_Depend = Arc<VelNode, AccNode>;
    using Ontology_Pos_Depend = Arc<PosNode, DisNode, Ontology_Vel_Depend>; // Graph Root
    struct PhysTag
    {
    };
    using Ontology_PhysGraph = Graph<PhysTag, Ontology_Pos_Depend>;

    using CollNode = Node<Collision>;
    using CollMaskNode = Node<CollisionMask>;
    using Ontology_Coll_Depend = Arc<CollNode, CollMaskNode>;
    struct CollTag{};
    using Ontology_CollGraph = Graph<CollTag, Ontology_Coll_Depend>;
    // meta graph
    using PhysGraphNode = Node<Ontology_PhysGraph>;
    using CollGraphNode = Node<Ontology_CollGraph>;
    using PhysGraphArc = Arc<PhysGraphNode, CollGraphNode>;
    struct PhysMetaGraphTag{};
    using PhysMetaGraph = Graph<PhysMetaGraphTag, PhysGraphArc>;
    
    
    // handle Graph as Node
    using GG_Graph_A_NR = Node<GG_A_Graph>;
    using GG_Graph_B_N0 = Node<GG_B_Graph>;
    using GG_Graph_C_N1 = Node<GG_C_Graph>;

    using GG_Graph_Arc = Arc<GG_Graph_A_NR, GG_Graph_B_N0, GG_Graph_C_N1>;
    struct Meta_Graphs_Tag
    {
    };
    using GG_Meta_Graph = Graph<Meta_Graphs_Tag, GG_Graph_Arc>;
} // namespace tsr