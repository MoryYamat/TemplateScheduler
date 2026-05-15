#include <iostream>

#include "graph_description.hpp"
#include "tsr/graph/graph_dsl.hpp"
#include "tsr/compiler/topological_sort.hpp"
// #include "executor.hpp"

#include "executors.hpp"
// build compile time dag

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


    std::cerr << "\n ===== HasPredecessor ==== \n";
    // IMPL 
    using force_check_impl_1 = HasPredecessorImpl<TP_NR, Relation<TP_N1, TP_N2>, ResolverDirection::RootFirst>;
    using force_check_impl_2 = HasPredecessorImpl<TP_N2, Relation<TP_NR, TP_N2>, ResolverDirection::RootFirst>;
    std::cerr << "HasPredecessorIMPL_Result_1 = " << force_check_impl_1::value << "\n";// 0
    std::cerr << "HasPredecessorIMPL_Result_2 = " << force_check_impl_2::value << "\n";// 1

    // IR
    // using force_check_1 = HasPredecessor<N_CRight_1, ResolverDirection::RootFirst, Relation<N_CRight_2, N_CCLeft_1>>;
    // using force_check_2 = HasPredecessor<N_CRight_1, ResolverDirection::RootFirst, Relation<N_CRight_2, N_CCLeft_1>, Relation<N_CRight_2, N_CRight_1>>;// 
    // std::cerr << "Result_Imp_1 = " << force_check_1::value << "\n";// 0
    // std::cerr << "Result_Imp_2 = " << force_check_2::value << "\n";// 1

    // IF
    using force_check_1 = HasPredecessor<TP_NR, RelationPack<Relation<TP_N1, TP_N2>>, ResolverDirection::RootFirst>;
    using force_check_2 = HasPredecessor<TP_N2, RelationPack<Relation<TP_N1, TP_N2>, Relation<TP_N3, TP_N4>>, ResolverDirection::RootFirst>;
    using force_check_3 = HasPredecessor<TP_NR, RelationPack<Relation<TP_NR, TP_N0>, Relation<TP_N0, TP_N1>, Relation<TP_N0, TP_N2>>, ResolverDirection::RootFirst>;
    std::cerr << "HasPredecessorIF_Result_1 = " << force_check_1::value << "\n";// 0
    std::cerr << "HasPredecessorIF_Result_2 = " << force_check_2::value << "\n";// 1
    std::cerr << "HasPredecessorIF_Result_2 = " << force_check_3::value << "\n";// 0

    using force_check_LeafFirst_1 = HasPredecessor<TP_NR, RelationPack<Relation<TP_N1, TP_N2>>, ResolverDirection::LeafFirst>;
    using force_check_LeafFirst_2 = HasPredecessor<TP_N2, RelationPack<Relation<TP_N1, TP_N2>, Relation<TP_N3, TP_N4>>, ResolverDirection::LeafFirst>;
    using force_check_LeafFirst_3 = HasPredecessor<TP_NR, RelationPack<Relation<TP_NR, TP_N0>, Relation<TP_N0, TP_N1>, Relation<TP_N0, TP_N2>>, ResolverDirection::LeafFirst>;
    std::cerr << "HasPredecessor_LeafFirst_Result_1 = " << force_check_LeafFirst_1::value << "\n";// 0
    std::cerr << "HasPredecessor_LeafFirst_Result_2 = " << force_check_LeafFirst_2::value << "\n";// 0
    std::cerr << "HasPredecessor_LeafFirst_Result_2 = " << force_check_LeafFirst_3::value << "\n";// 1

    std::cerr << "\n ============= AppendIfNoPredecessorNodes =============\n";
    using no_predecessor_nodes_1 = 
    AppendIfNoPredecessorNodes<NodePack<>, TP_NR, RelationPack<Relation<TP_NR, TP_N0>, Relation<TP_N0, TP_N1>, Relation<TP_N0, TP_N2>>, ResolverDirection::RootFirst>;

    using no_predecessor_nodes_2 = 
    AppendIfNoPredecessorNodes<NodePack<>, TP_N0, RelationPack<Relation<TP_NR, TP_N0>, Relation<TP_N0, TP_N1>, Relation<TP_N0, TP_N2>>, ResolverDirection::RootFirst>;
    std::cerr << "AppendIfNoPredecessorNodes_Result = " << typeid(no_predecessor_nodes_1::type).name() << "\n";// NodePack<TP_NR>
    std::cerr << "AppendIfNoPredecessorNodes_Result = " << typeid(no_predecessor_nodes_2::type).name() << "\n";// NodePack<>

    std::cerr << "\n ============= CollectReadyNodes =============\n";
    using collect_predecessor_nodes_1 = CollectReadyNodes<
    NodePack<TP_NR, TP_N0, TP_N1, TP_N2>, RelationPack<Relation<TP_NR, TP_N0>, Relation<TP_N0, TP_N1>, Relation<TP_N0, TP_N2>>, ResolverDirection::RootFirst>;
    using collect_predecessor_nodes_2 = CollectReadyNodes< 
    NodePack<TP_N0, TP_N1, TP_N2>, RelationPack<Relation<TP_NR, TP_N0>, Relation<TP_N0, TP_N1>, Relation<TP_N0, TP_N2>>, ResolverDirection::RootFirst>;
    std::cerr << "collect_predecessor_nodes_1_Result = " << typeid(collect_predecessor_nodes_1::type).name() << "\n";// NodePack<TP_NR>
    std::cerr << "collect_predecessor_nodes_2_Result = " << typeid(collect_predecessor_nodes_2::type).name() << "\n";// NodePack<>

    // Nodes for testing
    // TP_NR
    // TP_N0
    // TP_N1
    // TP_N2


    std::cerr << "\n ============= PruneRelationsByNode =============\n";
    using prune_relations_by_node_1 = PruneRelationsByNode<RelationPack<>, TP_NR, RelationPack<Relation<TP_NR, TP_N0>, Relation<TP_N0, TP_N1>, Relation<TP_N1,TP_N2>, Relation<TP_NR, TP_N3>>, ResolverDirection::RootFirst>::type;
    std::cerr << "prune_relations_by_node_1_Result = " << typeid(prune_relations_by_node_1).name() << "\n";// RelationPack<Relation<TP_N0, TP_N1>, Relation<TP_N1,TP_N2>>>
    
    std::cerr << "\n";
    // <list to delete node that is no predecessor of relation, RelationPack, Direction>
    using prune_relations_by_nodes_1 = PruneRelationsByNodes<NodePack<TP_NR, TP_N4>, RelationPack<Relation<TP_NR, TP_N0>, Relation<TP_N0, TP_N1>, Relation<TP_N1,TP_N2>, Relation<TP_NR, TP_N3>, Relation<TP_N4, TP_N5>>, ResolverDirection::RootFirst>::type;
    std::cerr << "prune_relations_by_nodes_1_Result = " << typeid(prune_relations_by_nodes_1).name() << "\n";// RelationPack<Relation<TP_N0, TP_N1>, Relation<TP_N1,TP_N2>>>

    std::cerr << "\n ============= PruneNode =============\n";
    using prune_nodes_by_nodes_1 = PruneNodeByNodes<NodePack<>/*resutl*/, NodePack<TP_NR>/*list to delete*/, NodePack<TP_NR, TP_N0, TP_N1, TP_N2, TP_N3>/*current node list*/>::type;
    std::cerr << "prune_nodes_by_node_1_Result = " << typeid(prune_nodes_by_nodes_1).name() << "\n";// NodePack<TP_N0, TP_N1, TP_N2, TP_N3>

    std::cerr << "\n ============= TOPOLOGICAL SORT ========== \n";
    using IR_TestGraph = typename Lower<TestGraph>::type;           // ############# Explicitly convert to IR ############# 
    using topological_sort_1 = TopologicalSort<IR_TestGraph, ResolverDirection::RootFirst>::type;
    using topological_sort_2 = TopologicalSort<IR_TestGraph, ResolverDirection::LeafFirst>::type;
    std::cerr << "topological_sort_1_Result = " << typeid(topological_sort_1).name() << "\n";
    std::cerr << "\ntopological_sort_2_Result = " << typeid(topological_sort_2).name() << "\n";

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

