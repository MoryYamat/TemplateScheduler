#include <iostream>

#include "graph_description.hpp"
#include "tsr/compiler/topological_sort.hpp"
#include "tsr/executor/executor.hpp"
#include "tsr/graph/graph_dsl.hpp"
// #include "executor.hpp"

#include "executors.hpp"
#include "tsr/ir/relation.hpp"
// build compile time dag

#include <vector>
#include <string>

int main()
{
    using namespace tsr;

    // std::cerr << "\n====== Ontology ========\n";
    // ResolveOrder<Ontology_PysGraph>::Resolve();

    // std::cerr << "\n====== Task Dependency ========\n";
    // ResolveOrder<TaskDepndency_PysGraph>::Resolve();

    // //
    // std::cerr << "\n====== Resolve Dependencies in the specified order ======\n";

    // std::cerr << "======= Root First ======= \n";
    // ResolveOrder<Ontology_PysGraph, ResolverDirection::RootFirst>::Resolve();

    // std::cerr << "======= Leaf First ======= \n";
    // ResolveOrder<Ontology_PysGraph, ResolverDirection::LeafFirst>::Resolve();

    // std::cerr << "\n ======= ResolverDirection TEST ======= \n";
    // ResolveOrder<TestGraph, ResolverDirection::RootFirst>::Resolve(0);
    // std::cerr << "\n";
    // ResolveOrder<TestGraph, ResolverDirection::LeafFirst>::Resolve(0);

    // std::cerr << "\n ===== HasPredecessor ==== \n";
    // // IMPL
    // using force_check_impl_1 = HasPredecessorImpl<TP_NR, Relation<TP_N1, TP_N2>, ResolverDirection::RootFirst>;
    // using force_check_impl_2 = HasPredecessorImpl<TP_N2, Relation<TP_NR, TP_N2>, ResolverDirection::RootFirst>;
    // std::cerr << "HasPredecessorIMPL_Result_1 = " << force_check_impl_1::value << "\n";// 0
    // std::cerr << "HasPredecessorIMPL_Result_2 = " << force_check_impl_2::value << "\n";// 1

    // // IR
    // // using force_check_1 = HasPredecessor<N_CRight_1, ResolverDirection::RootFirst, Relation<N_CRight_2, N_CCLeft_1>>;
    // // using force_check_2 = HasPredecessor<N_CRight_1, ResolverDirection::RootFirst, Relation<N_CRight_2, N_CCLeft_1>, Relation<N_CRight_2, N_CRight_1>>;//
    // // std::cerr << "Result_Imp_1 = " << force_check_1::value << "\n";// 0
    // // std::cerr << "Result_Imp_2 = " << force_check_2::value << "\n";// 1

    // // IF
    // using force_check_1 = HasPredecessor<TP_NR, RelationPack<Relation<TP_N1, TP_N2>>, ResolverDirection::RootFirst>;
    // using force_check_2 = HasPredecessor<TP_N2, RelationPack<Relation<TP_N1, TP_N2>, Relation<TP_N3, TP_N4>>, ResolverDirection::RootFirst>;
    // using force_check_3 = HasPredecessor<TP_NR, RelationPack<Relation<TP_NR, TP_N0>, Relation<TP_N0, TP_N1>, Relation<TP_N0, TP_N2>>, ResolverDirection::RootFirst>;
    // std::cerr << "HasPredecessorIF_Result_1 = " << force_check_1::value << "\n";// 0
    // std::cerr << "HasPredecessorIF_Result_2 = " << force_check_2::value << "\n";// 1
    // std::cerr << "HasPredecessorIF_Result_2 = " << force_check_3::value << "\n";// 0

    // using force_check_LeafFirst_1 = HasPredecessor<TP_NR, RelationPack<Relation<TP_N1, TP_N2>>, ResolverDirection::LeafFirst>;
    // using force_check_LeafFirst_2 = HasPredecessor<TP_N2, RelationPack<Relation<TP_N1, TP_N2>, Relation<TP_N3, TP_N4>>, ResolverDirection::LeafFirst>;
    // using force_check_LeafFirst_3 = HasPredecessor<TP_NR, RelationPack<Relation<TP_NR, TP_N0>, Relation<TP_N0, TP_N1>, Relation<TP_N0, TP_N2>>, ResolverDirection::LeafFirst>;
    // std::cerr << "HasPredecessor_LeafFirst_Result_1 = " << force_check_LeafFirst_1::value << "\n";// 0
    // std::cerr << "HasPredecessor_LeafFirst_Result_2 = " << force_check_LeafFirst_2::value << "\n";// 0
    // std::cerr << "HasPredecessor_LeafFirst_Result_2 = " << force_check_LeafFirst_3::value << "\n";// 1

    // std::cerr << "\n ============= AppendIfNoPredecessorNodes =============\n";
    // using no_predecessor_nodes_1 =
    // AppendIfNoPredecessorNodes<NodePack<>, TP_NR, RelationPack<Relation<TP_NR, TP_N0>, Relation<TP_N0, TP_N1>, Relation<TP_N0, TP_N2>>, ResolverDirection::RootFirst>;

    // using no_predecessor_nodes_2 =
    // AppendIfNoPredecessorNodes<NodePack<>, TP_N0, RelationPack<Relation<TP_NR, TP_N0>, Relation<TP_N0, TP_N1>, Relation<TP_N0, TP_N2>>, ResolverDirection::RootFirst>;
    // std::cerr << "AppendIfNoPredecessorNodes_Result = " << typeid(no_predecessor_nodes_1::type).name() << "\n";// NodePack<TP_NR>
    // std::cerr << "AppendIfNoPredecessorNodes_Result = " << typeid(no_predecessor_nodes_2::type).name() << "\n";// NodePack<>

    // std::cerr << "\n ============= CollectReadyNodes =============\n";
    // using collect_predecessor_nodes_1 = CollectReadyNodes<
    // NodePack<TP_NR, TP_N0, TP_N1, TP_N2>, RelationPack<Relation<TP_NR, TP_N0>, Relation<TP_N0, TP_N1>, Relation<TP_N0, TP_N2>>, ResolverDirection::RootFirst>;
    // using collect_predecessor_nodes_2 = CollectReadyNodes<
    // NodePack<TP_N0, TP_N1, TP_N2>, RelationPack<Relation<TP_NR, TP_N0>, Relation<TP_N0, TP_N1>, Relation<TP_N0, TP_N2>>, ResolverDirection::RootFirst>;
    // std::cerr << "collect_predecessor_nodes_1_Result = " << typeid(collect_predecessor_nodes_1::type).name() << "\n";// NodePack<TP_NR>
    // std::cerr << "collect_predecessor_nodes_2_Result = " << typeid(collect_predecessor_nodes_2::type).name() << "\n";// NodePack<>

    // Nodes for testing
    // TP_NR
    // TP_N0
    // TP_N1
    // TP_N2

    // std::cerr << "\n ============= PruneRelationsByNode =============\n";
    // using prune_relations_by_node_1 = PruneRelationsByNode<RelationPack<>, TP_NR, RelationPack<Relation<TP_NR, TP_N0>, Relation<TP_N0, TP_N1>, Relation<TP_N1,TP_N2>, Relation<TP_NR, TP_N3>>, ResolverDirection::RootFirst>::type;
    // std::cerr << "prune_relations_by_node_1_Result = " << typeid(prune_relations_by_node_1).name() << "\n";// RelationPack<Relation<TP_N0, TP_N1>, Relation<TP_N1,TP_N2>>>

    // std::cerr << "\n";
    // // <list to delete node that is no predecessor of relation, RelationPack, Direction>
    // using prune_relations_by_nodes_1 = PruneRelationsByNodes<NodePack<TP_NR, TP_N4>, RelationPack<Relation<TP_NR, TP_N0>, Relation<TP_N0, TP_N1>, Relation<TP_N1,TP_N2>, Relation<TP_NR, TP_N3>, Relation<TP_N4, TP_N5>>, ResolverDirection::RootFirst>::type;
    // std::cerr << "prune_relations_by_nodes_1_Result = " << typeid(prune_relations_by_nodes_1).name() << "\n";// RelationPack<Relation<TP_N0, TP_N1>, Relation<TP_N1,TP_N2>>>

    // std::cerr << "\n ============= PruneNode =============\n";
    // using prune_nodes_by_nodes_1 = PruneNodeByNodes<NodePack<>/*resutl*/, NodePack<TP_NR>/*list to delete*/, NodePack<TP_NR, TP_N0, TP_N1, TP_N2, TP_N3>/*current node list*/>::type;
    // std::cerr << "prune_nodes_by_node_1_Result = " << typeid(prune_nodes_by_nodes_1).name() << "\n";// NodePack<TP_N0, TP_N1, TP_N2, TP_N3>

    std::cerr << "\n ============= TOPOLOGICAL SORT ========== \n";
    using IR_TestGraph = typename Lower<TestGraph>::type; // ############# Explicitly convert to IR #############
    using topological_sort_1 = TopologicalSort<IR_TestGraph, ResolverDirection::RootFirst>::type;
    using topological_sort_2 = TopologicalSort<IR_TestGraph, ResolverDirection::LeafFirst>::type;
    // std::cerr << "topological_sort_1_Result = " << typeid(topological_sort_1).name() << "\n";
    // std::cerr << "\ntopological_sort_2_Result = " << typeid(topological_sort_2).name() << "\n";

    // std::cerr << "\n ============== CYcle detection ========== \n";
    // be detected by `static_assert(!std::is_same_v<ready, NodePack<>>, "Cycle detected or graph is not topologically sortable");`
    // using topological_sort_cyclic_1 = TopologicalSort<Cy_Graph, ResolverDirection::RootFirst>::type;
    // using topological_sort_cyclic_2 = TopologicalSort<Cy_Graph, ResolverDirection::LeafFirst>::type;

    std::cerr << "\n ============== Exectution Order Test ========= \n";
    Context ctx{};
    std::cerr << "\n RootFirst:";
    ExecuteOrder<topological_sort_1>::Run(ctx);

    std::cerr << "\n LeafFirst:";
    ExecuteOrder<topological_sort_2>::Run(ctx);

    std::cerr << "\n ================ Test of Graph of Graphs ===============\n ";
    using IR_GG_META_GRAPH_Hierarchical_RFRF =
        typename MakeHierarchicalPlan<GG_Meta_Graph, ResolverDirection::RootFirst, ResolverDirection::RootFirst,
                                      PlanKind::Sequential>::type;
    using IR_GG_META_GRAPH_Hierarchical_RFLF =
        typename MakeHierarchicalPlan<GG_Meta_Graph, ResolverDirection::RootFirst, ResolverDirection::LeafFirst,
                                      PlanKind::Sequential>::type;
    using IR_GG_META_GRAPH_Hierarchical_Overrides_LFRF =
        typename MakeHierarchicalPlan<GG_Meta_Graph, ResolverDirection::RootFirst, ResolverDirection::RootFirst,
                                      PlanKind::Sequential,
                                      GraphDirection<GG_A_Graph, ResolverDirection::LeafFirst>>::type;
    std::cerr << "IR_GG_META_GRAPH_Hierarchical_RFRF: \n" << typeid(IR_GG_META_GRAPH_Hierarchical_RFRF).name() << "\n";
    std::cerr << "\nIR_GG_META_GRAPH_Hierarchical_RFLF: \n"
              << typeid(IR_GG_META_GRAPH_Hierarchical_RFLF).name() << "\n";
    std::cerr << "\nIR_GG_META_GRAPH_Hierarchical_Overrides_LFRF: \n"
              << typeid(IR_GG_META_GRAPH_Hierarchical_Overrides_LFRF).name() << "\n";

    // Test for Sequential Plans
    struct R
    {
    };
    struct L1
    {
    };
    struct L2
    {
    };
    struct Tag
    {
    };
    using G = Graph<Tag, Arc<Node<R>, Node<L1>, Node<L2>>>;
    using Expected = SequentialPlan<NodePack<Node<R>, Node<L1>, Node<L2>>>;
    using Actual = typename MakeSequentialPlan<G, ResolverDirection::RootFirst>::type;
    static_assert(std::is_same_v<Actual, Expected>);

    // Test for HierarchicalPlan
    // -------------------- Test nodes --------------------
    struct A0
    {
    };
    struct A1
    {
    };
    struct B0
    {
    };
    struct B1
    {
    };

    struct ATag
    {
    };
    struct BTag
    {
    };
    struct MetaTag
    {
    };

    // Sub graphs
    using GraphA = tsr::Graph<ATag, tsr::Arc<tsr::Node<A0>, tsr::Node<A1>>>;

    using GraphB = tsr::Graph<BTag, tsr::Arc<tsr::Node<B0>, tsr::Node<B1>>>;

    //  Meta graph
    using MetaGraph = tsr::Graph<MetaTag, tsr::Arc<tsr::Node<GraphA>, tsr::Node<GraphB>>>;

    //  Expected
    using ExpectedMetaPlan = tsr::HierarchicalPlan<
        tsr::SequentialPlan<tsr::NodePack<tsr::Node<GraphA>, tsr::Node<GraphB>>>,
        tsr::SubPlanPack<tsr::SubPlan<ATag, GraphA, tsr::SequentialPlan<tsr::NodePack<tsr::Node<A0>, tsr::Node<A1>>>>,
                         tsr::SubPlan<BTag, GraphB, tsr::SequentialPlan<tsr::NodePack<tsr::Node<B0>, tsr::Node<B1>>>>>>;

    // Actual
    using ActualMetaPlan =
        typename tsr::MakeHierarchicalPlan<MetaGraph, tsr::ResolverDirection::RootFirst,
                                           tsr::ResolverDirection::RootFirst, PlanKind::Sequential>::type;

    // Test
    static_assert(std::is_same_v<ActualMetaPlan, ExpectedMetaPlan>, "MakeHierarchicalPlan RootFirst/RootFirst failed");
    // ----------------------------------------------------------------------------------------------------

    // ----------------------------------------- Test For Hierarchical Plan -----------------------------------------
    std::cerr << "\n ================== Executing Hierarchical Plan Tests ================== \n";
    using PysDomainHierarchicalPlan = MakeHierarchicalPlan<PhysMetaGraph, ResolverDirection::RootFirst,
                                                           ResolverDirection::LeafFirst, PlanKind::Sequential>::type;
    ExecutePlan<PysDomainHierarchicalPlan, WarnExecutionConfig>::Run(ctx);
    // ######## Exepected ########
    using ExpectedPysMetaPlan = SequentialPlan<NodePack<Node<Ontology_PhysGraph>, Node<Ontology_CollGraph>>>;
    using ExepectedPhysicsSubPlan =
        SubPlan<PhysTag, Ontology_PhysGraph, SequentialPlan<NodePack<Node<Dis>, Node<Acc>, Node<Vel>, Node<Pos>>>>;
    using ExepectedCollisionSubPlan =
        SubPlan<CollTag, Ontology_CollGraph, SequentialPlan<NodePack<Node<CollisionMask>, Node<Collision>>>>;

    using ExpectedPhyDomainPlan =
        HierarchicalPlan<ExpectedPysMetaPlan, SubPlanPack<ExepectedPhysicsSubPlan, ExepectedCollisionSubPlan>>;
    static_assert(std::is_same_v<PysDomainHierarchicalPlan, ExpectedPhyDomainPlan>, "Hierarchical plan mismatch");

    // fallback static Run() を持つ Executor 特殊化なしで実行可能
    struct HasRunNoCtx
    {
        static void Run() {}
    };
    static_assert(HasStaticRunNoContext<HasRunNoCtx>);
    // fallback: static Run(Context&)
    struct HasRunCtx
    {
        static void Run(Context&) {}
    };
    static_assert(HasStaticRunWithContext<HasRunCtx, Context>);
    // 3. Runを持たない型は検出されない
    struct NoRun
    {
    };
    static_assert(!HasStaticRunNoContext<NoRun>);
    static_assert(!HasStaticRunWithContext<NoRun, tsr::Context>);
    // Test ExecutePlan with MissingPolicy
    using WarnPlan = SequentialPlan<NodePack<Node<NoRun>, Node<HasRunNoCtx>, Node<HasRunCtx>>>;
    std::cerr << "\n === MissingExecutorPolicy TEST === \n";
    ExecutePlan<WarnPlan, WarnExecutionConfig>::Run(ctx);

    // ====================== DISPATCH PRIORITY TEST ======================
    std::cerr << "\n === DISPATCH PRIORITY TEST === \n";
    using Dispatch_Priority_Test_Plan = SequentialPlan<NodePack<Node<A>, Node<B>, Node<C>, Node<D>>>;
    TestContext test_ctx{};
    ExecutePlan<Dispatch_Priority_Test_Plan, WarnExecutionConfig>::Run(test_ctx);
    static_assert(HasStaticRunNoContext<D>);
    for (const auto& u : test_ctx.log)
    {
        std::cerr << u << "\n";
    }
    std::cerr << "D called = " << D::called << "\n";

    // std::cerr << "\nGG_A_Graph : \n"  << typeid(TopologicalSort<Lower<GG_A_Graph>::type, ResolverDirection::RootFirst>::type).name() << "\n";
    // std::cerr << "\nIR_GG_META_GRAPH_meta_plan_LF: \n" << typeid(IR_GG_META_GRAPH_meta_plan_LF).name() << "\n";// Intermediate results

    // using topo_IR_Test_Meta_Graph = typename TopologicalSort<GG_Meta_Graph, ResolverDirection::RootFirst>::type;
    // std::cerr << "topo_IR_Test_Meta_Graph_Result = " << typeid(topo_IR_Test_Meta_Graph).name() << "\n";

    std::cerr << "\n";

    // ============= Append Unique Nodes From Elements ============
    //using uqniue_nodes_result = Lower<TestGraph>::nodes;
    using LayeredPlan_Test_RF = typename MakeLayeredPlan<LP_G, ResolverDirection::RootFirst>::type;
    using LayeredPlan_Test_LF = typename MakeLayeredPlan<LP_G, ResolverDirection::LeafFirst>::type;
    using LP_G_MakeLayered_RF_Expected =
        LayeredPlan<LayerPack<NodePack<Node<LP_R>, Node<LP_C>>, NodePack<Node<LP_A>, Node<LP_B>>>>;
    using LP_G_MakeLayered_LF_Expected =
        LayeredPlan<LayerPack<NodePack<Node<LP_A>, Node<LP_B>>, NodePack<Node<LP_R>, Node<LP_C>>>>;
    static_assert(std::is_same_v<LayeredPlan_Test_RF, LP_G_MakeLayered_RF_Expected>);
    static_assert(std::is_same_v<LayeredPlan_Test_LF, LP_G_MakeLayered_LF_Expected>);

    ExecutePlan<LayeredPlan_Test_RF, DebugExecutionConfig>::Run(ctx);

    std::cerr << "\n";
    // ============= META PLAN + LAYRED PLAN ================
    // ================================================================
    // HierarchicalPlan RootFirst + Sequential
    // ================================================================
    using Actual_Hierarchical_RF_RF =
        typename MakeHierarchicalPlan<GG_Meta_Graph, ResolverDirection::RootFirst, ResolverDirection::RootFirst,
                                      PlanKind::Sequential>::type;
    using Expected_Hierarchical_RF_RF = HierarchicalPlan<
        SequentialPlan<NodePack<Node<GG_A_Graph>, Node<GG_B_Graph>, Node<GG_C_Graph>>>,
        SubPlanPack<
            SubPlan<GG_TEST_A_GraphTag, GG_A_Graph,
                    SequentialPlan<NodePack<Node<GG_A_R>, Node<GG_A_0>, Node<GG_A_1>, Node<GG_A_2>, Node<GG_A_3>>>>,
            SubPlan<GG_TEST_B_GraphTag, GG_B_Graph,
                    SequentialPlan<NodePack<Node<GG_B_R>, Node<GG_B_0>, Node<GG_B_1>, Node<GG_B_2>, Node<GG_B_3>>>>,
            SubPlan<GG_TEST_C_GraphTag, GG_C_Graph,
                    SequentialPlan<NodePack<Node<GG_C_R>, Node<GG_C_0>, Node<GG_C_1>, Node<GG_C_2>, Node<GG_C_3>>>>>>;

    static_assert(std::is_same_v<Actual_Hierarchical_RF_RF, Expected_Hierarchical_RF_RF>,
                  "HierarchicalPlan Sequential failed");

    // ================================================================
    // HierarchicalPlan RootFirst + Layered
    // ================================================================
    using Actual_Hierarchical_RF_Layered_HL =
        typename MakeHierarchicalPlan<GG_Meta_Graph_HL, ResolverDirection::RootFirst, ResolverDirection::RootFirst,
                                      PlanKind::Layered>::type;
    using Expected_Hierarchical_RF_Layered = HierarchicalPlan<
        SequentialPlan<NodePack<Node<GG_A_Graph_HL>, Node<GG_B_Graph_HL>, Node<GG_C_Graph_HL>>>,
        SubPlanPack<SubPlan<GG_TEST_A_GraphTag, GG_A_Graph_HL,
                            LayeredPlan<LayerPack<NodePack<Node<GG_A_R>>, NodePack<Node<GG_A_0>, Node<GG_A_1>>,
                                                  NodePack<Node<GG_A_2>, Node<GG_A_3>>>>>,
                    SubPlan<GG_TEST_B_GraphTag, GG_B_Graph_HL,
                            LayeredPlan<LayerPack<NodePack<Node<GG_B_R>>, NodePack<Node<GG_B_0>, Node<GG_B_1>>,
                                                  NodePack<Node<GG_B_2>, Node<GG_B_3>>>>>,
                    SubPlan<GG_TEST_C_GraphTag, GG_C_Graph_HL,
                            LayeredPlan<LayerPack<NodePack<Node<GG_C_R>>, NodePack<Node<GG_C_0>, Node<GG_C_1>>,
                                                  NodePack<Node<GG_C_2>, Node<GG_C_3>>>>>>>;

    static_assert(std::is_same_v<Actual_Hierarchical_RF_Layered_HL, Expected_Hierarchical_RF_Layered>,
                  "HierarchicalPlan Layered failed");

    // ================================================================
    // HierarchicalPlan Override Test
    // Graph A only -> LeafFirst + Layered
    // ================================================================
    using Actual_Hierarchical_Override_HL =
        typename MakeHierarchicalPlan<GG_Meta_Graph, ResolverDirection::RootFirst, ResolverDirection::RootFirst,
                                      PlanKind::Sequential, GraphDirection<GG_A_Graph_HL, ResolverDirection::LeafFirst>,
                                      GraphPlanKind<GG_A_Graph_HL, PlanKind::Layered>>::type;
    using Expected_Hierarchical_Override = HierarchicalPlan<
        SequentialPlan<NodePack<Node<GG_A_Graph_HL>, Node<GG_B_Graph_HL>, Node<GG_C_Graph_HL>>>,
        SubPlanPack<
            SubPlan<GG_TEST_A_GraphTag, GG_A_Graph_HL,
                    LayeredPlan<LayerPack<NodePack<Node<GG_A_0>, Node<GG_A_2>, Node<GG_A_3>>,
                                          NodePack<Node<GG_A_1>>,
                                          NodePack<Node<GG_A_R>>>>>,
            SubPlan<GG_TEST_B_GraphTag, GG_B_Graph_HL,
                    SequentialPlan<NodePack<Node<GG_B_R>, Node<GG_B_0>, Node<GG_B_1>, Node<GG_B_2>, Node<GG_B_3>>>>,
            SubPlan<GG_TEST_C_GraphTag, GG_C_Graph_HL,
                    SequentialPlan<NodePack<Node<GG_C_R>, Node<GG_C_0>, Node<GG_C_1>, Node<GG_C_2>, Node<GG_C_3>>>>>>;

    static_assert(std::is_same_v<Actual_Hierarchical_Override_HL, Expected_Hierarchical_Override>,
                  "HierarchicalPlan Override failed");

    // std::cerr << "LayeredPlan_Test_RF_Result = " << typeid(LayeredPlan_Test_RF).name() << "\n";
    // std::cerr << "\nLayeredPlan_Test_LF_Result = " << typeid(LayeredPlan_Test_LF).name() << "\n";
    //std::cerr << "uqniue_nodes_result = " << typeid(uqniue_nodes_result).name() << "\n";

    // using LayeredPlan_Test = MakeLayeredPlan<LP_G, ResolverDirection::RootFirst>::type;
    // std::cerr << "LayeredPlan_Test_Result = " = typeid(LayeredPlan_Test).name() << "\n";
    // ExecutePlan<LayeredPlan_Test, WarnExecutionConfig>::Run(ctx);

    // confirmed
    // using test_make_subPlan_RF = typename MakeSubPlan<TestGraph, ResolverDirection::RootFirst>::type;
    // std::cerr << "test_make_subPlan_RF_Result = " << typeid(test_make_subPlan_RF).name() << "\n";

    // using sq_plan_test_RF = typename MakeSequentialPlan<IR_TestGraph, ResolverDirection::RootFirst>::type;
    // std::cerr << "sq_plan_test_Result = " << typeid(sq_plan_test_RF).name() << "\n";
    // using sq_plan_test_LF = typename MakeSequentialPlan<IR_TestGraph, ResolverDirection::LeafFirst>::type;
    // std::cerr << "\nsq_plan_test_Result = " << typeid(sq_plan_test_LF).name() << "\n";

    //
    //R_Test::Print();
    // Type Force Checker (force instanciation)
    // using ForceCheck = typename Ontology_PysGraph::element_types;
    // using ForceCheck = typename TestGraph::element_types;
    // using ForceCheck = typename TestGraph::relations;
    // using ForceCheck_Arc = typename A_CCRight::branches;// OK
    // using ForceCheck_Arc = typename A_CCRight_Error::branches;
    // using ForceCheck_Arc = typename A_CCRithg_Error_Dup::branches;

    return 0;
}

// ### 0.
// - Resolver<FooGraph::Bar>->
