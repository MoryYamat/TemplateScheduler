#include <iostream>

#include "ComponentHierarchyDescription.h"
#include "HierarchyConstructor.h"

// build compile time dag

int main()
{
    using namespace tsr;

    std::cerr << "\n====== Ontology ========\n";
    ResolveOrder<Ontology_PysGraph>::Resolve();

    std::cerr << "\n====== Task Dependency ========\n";
    ResolveOrder<TaskDepndency_PysGraph>::Resolve();


    // 
    std::cerr << "\n====== Resolve Dependencies in the specified order ======\n";

    std::cerr << "======= Root First ======= \n";
    ResolveOrder<Ontology_PysGraph, ResolverDirection::RootFirst>::Resolve();

    
    std::cerr << "======= Leaf First ======= \n";
    ResolveOrder<Ontology_PysGraph, ResolverDirection::LeafFirst>::Resolve();

    std::cerr << "\n ======= Test ======= \n";
    ResolveOrder<TestGraph, ResolverDirection::RootFirst>::Resolve(0);
    std::cerr << "\n";
    ResolveOrder<TestGraph, ResolverDirection::LeafFirst>::Resolve(0);

    
    // std::cerr << "\n ======= Cyclic ====== \n";
    // ResolveOrder<Cy_Graph, ResolverDirection::RootFirst>::Resolve(0);
    // ResolveOrder<Cy_Graph, ResolverDirection::LeafFirst>::Resolve(0);

    // relation test
    std::cerr << "\n ===== Relation Test ===== \n";

    std::cerr << "\n ===== Root First ===== \n";
    TopologicalOrder<TestGraph, ResolverDirection::RootFirst>::Print();

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

    using collect_predecessor_nodes_1 = CollectReadyNodes<
    NodePack<TP_NR, TP_N0, TP_N1, TP_N2>, RelationPack<Relation<TP_NR, TP_N0>, Relation<TP_N0, TP_N1>, Relation<TP_N0, TP_N2>>, ResolverDirection::RootFirst>;
    using collect_predecessor_nodes_2 = CollectReadyNodes< 
    NodePack<TP_N0, TP_N1, TP_N2>, RelationPack<Relation<TP_NR, TP_N0>, Relation<TP_N0, TP_N1>, Relation<TP_N0, TP_N2>>, ResolverDirection::RootFirst>;
    std::cerr << "collect_predecessor_nodes_1_Result = " << typeid(collect_predecessor_nodes_1::type).name() << "\n";// NodePack<TP_NR>
    std::cerr << "collect_predecessor_nodes_2_Result = " << typeid(collect_predecessor_nodes_2::type).name() << "\n";// NodePack<>

    // N_CRight_1
    // N_CRight_2
    // N_CCLeft_1
    // N_CCLeft_2
    // N_CCRight_1
    // N_CCCLeft_1
    // N_CCCRight_1



    std::cerr << "\n ===== Leaf First =====\n";
    // TopologicalOrder<TestGraph, ResolverDirection::LeafFirst>::Print();
    //TestGraph::Print();
    
    //R_Test::Print();
    // Type Force Checker
    // using ForceCheck = typename Ontology_PysGraph::element_types;
    // using ForceCheck = typename TestGraph::element_types;
    using ForceCheck = typename TestGraph::relations;
    // using ForceCheck_Arc = typename A_CCRight::branches;// OK
    // using ForceCheck_Arc = typename A_CCRight_Error::branches;
    // using ForceCheck_Arc = typename A_CCRithg_Error_Dup::branches;

    return 0;
}

// ### 0.
// - Resolver<FooGraph::Bar>-> 

