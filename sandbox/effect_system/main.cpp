#include <iostream>
#include <type_traits>

#include "tsr/effects/effect.hpp"// dsl
#include "tsr/effects/plan.hpp"// compiler 

#include "dsl_descriptions.hpp"// Required for extraction
#include "dsl_effects.hpp"// Required for extraction


int main()
{
    std::cout << "hello effect system\n";

    using namespace tsr;
    using namespace es;

    using CES_EXE_SET = ExecutionSet<CES_PROC_N_INTENT, CES_PROC_N_INTEG,CES_PROC_N_COLL,CES_PROC_N_CAMERA, CES_PROC_N_RENDER>;
    using Expected_CES_EXE_SET = ExecutionSet<Node<IntentSystem>, Node<Integrate>, Node<CollisionDetection>, Node<CameraController>, Node<Renderer>>;
    static_assert(std::is_same_v<Expected_CES_EXE_SET, CES_EXE_SET>, "Error in `Execution Set`");

    // extractors
    // reads
    using CES_EXTRACT_Reads_FROM_Effects_INTEG = typename ExtractReads<Effects<Integrate>>::type;
    using Expected_CES_EXTRACT_Reads_INTEG = ResourcePack<es::Intent, es::Velocity, es::Acceleration>;
    using CES_EXTRACT_Reads_FROM_Node_INTEG = typename ExtractReads<Node<Integrate>>::type;
    static_assert(std::is_same_v<Expected_CES_EXTRACT_Reads_INTEG, CES_EXTRACT_Reads_FROM_Effects_INTEG>, "Error in `ExtractReads`");
    static_assert(std::is_same_v<Expected_CES_EXTRACT_Reads_INTEG, CES_EXTRACT_Reads_FROM_Node_INTEG>, "Error in `ExtractReads");
    // std::cerr << "Extract From Effects = " << typeid(CES_EXTRACT_Reads_FROM_Effects_INTEG).name() << "\n";
    // std::cerr << "Extract From Node = " << typeid(CES_EXTRACT_Reads_FROM_Node_INTEG).name() << "\n";
    // writes
    using CES_EXTRACT_Writes_FROM_Effects_INTEG = typename ExtractWrites<Effects<Integrate>>::type;
    using CES_EXTRACT_Writes_FROM_Node_INTEG = typename ExtractWrites<Node<Integrate>>::type;
    using Expected_CES_EXTRACT_Writes_INTEG = ResourcePack<es::Position>;
    static_assert(std::is_same_v<Expected_CES_EXTRACT_Writes_INTEG, CES_EXTRACT_Writes_FROM_Effects_INTEG>, "Error in `ExtractWrites`");
    static_assert(std::is_same_v<Expected_CES_EXTRACT_Writes_INTEG, CES_EXTRACT_Writes_FROM_Node_INTEG>, "Error in `ExtractWrites");

    using CES_HAS_RESOURCE_INTERSECTION_INTEG = HasReadWriteConflict<Effects<Integrate>, Effects<CollisionDetection>>;      // Effects<Integrate>::reads && Effects<CollisionDetection>::writes -> Empty()
    static_assert(!CES_HAS_RESOURCE_INTERSECTION_INTEG::type::value, "Error in `DetermineReadWrite`");                      // expected == False

    // <Integrate>::writes::position and <CollisionDetection>::reads::position conflict
    static_assert(!CanRunTogether_v<CES_PROC_N_INTEG, CES_PROC_N_COLL>, "Error in `CanRunTogether" );                 // expected == False 

    using CES_APPEND_IF_CONFLICTS = typename AppendConflictRelation<RelationPack<>, CES_PROC_N_INTEG, CES_PROC_N_COLL>::type;
    // std::cerr << "CES_APPEND_IF_CONFLICTS Result = " << typeid(CES_APPEND_IF_CONFLICTS).name() << "\n";
    using CES_COLLECT_CONFLICTS_FOR_NODE_Res = typename CollectConflictRelations<RelationPack<>, CES_EXE_SET>::type;
    //std::cerr << "CES_COLLECT_CONFLICTS_FOR_NODE_Res = " << typeid(CES_COLLECT_CONFLICTS_FOR_NODE_Res).name() << "\n";
    using Exepected_CES_COLLECT_CONFLICTS_FOR_NODE_Res =    RelationPack<Relation<Node<IntentSystem>, Node<Integrate>>,Relation<Node<Integrate>, Node<CollisionDetection>>, 
                                                            Relation<Node<Integrate>, Node<CameraController>>, Relation<Node<Integrate>, Node<Renderer>>,
                                                            Relation<Node<CameraController>, Node<Renderer>>>;
    static_assert(std::is_same_v<Exepected_CES_COLLECT_CONFLICTS_FOR_NODE_Res, CES_COLLECT_CONFLICTS_FOR_NODE_Res>, "Error in `CollectEffectRelations`");
    
    struct CES_TestTag{};
    using CES_SAFE_LAYERED_PLAN_RF = typename MakeSafeLayeredPlan<CES_TestTag, CES_EXE_SET, ResolverDirection::RootFirst>::type;
    // std::cerr <<  "CES_SAFE_LAYERED_PLAN_RF Result = " << typeid(CES_SAFE_LAYERED_PLAN).name() << "\n";
    using Expected_CES_SAFE_LAYERED_PLAN_RF = SafeLayeredPlan<LayerPack<NodePack<Node<IntentSystem>>, 
                                                                     NodePack<Node<Integrate>>, 
                                                                     NodePack<Node<CollisionDetection>, Node<CameraController>>,
                                                                     NodePack<Node<Renderer>>>>;
    static_assert(std::is_same_v<Expected_CES_SAFE_LAYERED_PLAN_RF, CES_SAFE_LAYERED_PLAN_RF >, "Error in `MakeSafelayeredPlan`");
    // ############# Deprecated ############# 
    // using CES_SAFE_LAYERED_PLAN_LF = typename MakeSafeLayeredPlan<CES_TestTag, CES_EXE_SET, ResolverDirection::LeafFirst>::type; LeafFirst is currently deprecated in Plans derived from EffectSystem.
    // using Expected_CES_SAFE_LAYERED_PLAN_LF = SafeLayeredPlan<LayerPack<NodePack<Node<CollisionDetection>,Node<Renderer>>, 
    //                                                                     NodePack<Node<CameraController>>,
    //                                                                     NodePack<Node<Integrate>>, 
    //                                                                     NodePack<Node<IntentSystem>>>>;
    // std::cerr <<  "CES_SAFE_LAYERED_PLAN_LF Result = " << typeid(CES_SAFE_LAYERED_PLAN_LF).name() << "\n";
    // static_assert(std::is_same_v<Expected_CES_SAFE_LAYERED_PLAN_LF, CES_SAFE_LAYERED_PLAN_LF >, "Error in `MakeSafelayeredPlan`");// Deprecated 

    return 0;
}