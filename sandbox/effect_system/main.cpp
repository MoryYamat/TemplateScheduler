#include <iostream>
#include <type_traits>

#include "tsr/effects/effect.hpp"      // dsl
#include "tsr/plan/plan.hpp"           // required for making plan
#include "tsr/executor/executor.hpp"   // required for execution plans
#include "tsr/effects/validation.hpp"  // Required for verification testing
#include "tsr/analysis/plan_stats.hpp" // Required for plan stats
#include "tsr/analysis/plan_analyzer.hpp"

#include "dsl_descriptions.hpp" // Required for extraction
#include "dsl_effects.hpp"      // Required for extraction

#include "tsr/runtime/thread_pool.hpp"

#include "user_thread_pool.hpp"
#include "dsl_thread_pool_adapter.hpp" // Required for adapting to a thread pool.

#include "tsr/visualizer/visualizer.hpp"
#include "tsr/validator/plan_validation.hpp"

int main()
{
    // std::cout << "hello effect system\n";

    using namespace tsr;
    using namespace es;

    using CES_EXE_SET =
        ExecutionSet<CES_PROC_N_INTENT, CES_PROC_N_INTEG, CES_PROC_N_COLL, CES_PROC_N_CAMERA, CES_PROC_N_RENDER>;
    using Expected_CES_EXE_SET = ExecutionSet<Node<IntentSystem>, Node<Integrate>, Node<CollisionDetection>,
                                              Node<CameraController>, Node<Renderer>>;
    static_assert(std::is_same_v<Expected_CES_EXE_SET, CES_EXE_SET>, "Error in `Execution Set`");

    // extractors
    // reads
    using CES_EXTRACT_Reads_FROM_Effects_INTEG = typename ExtractReads<Effects<Integrate>>::type;
    using Expected_CES_EXTRACT_Reads_INTEG = ResourcePack<es::Intent, es::Velocity, es::Acceleration>;
    using CES_EXTRACT_Reads_FROM_Node_INTEG = typename ExtractReads<Node<Integrate>>::type;
    static_assert(std::is_same_v<Expected_CES_EXTRACT_Reads_INTEG, CES_EXTRACT_Reads_FROM_Effects_INTEG>,
                  "Error in `ExtractReads`");
    static_assert(std::is_same_v<Expected_CES_EXTRACT_Reads_INTEG, CES_EXTRACT_Reads_FROM_Node_INTEG>,
                  "Error in `ExtractReads");
    // std::cerr << "Extract From Effects = " << typeid(CES_EXTRACT_Reads_FROM_Effects_INTEG).name() << "\n";
    // std::cerr << "Extract From Node = " << typeid(CES_EXTRACT_Reads_FROM_Node_INTEG).name() << "\n";
    // writes
    using CES_EXTRACT_Writes_FROM_Effects_INTEG = typename ExtractWrites<Effects<Integrate>>::type;
    using CES_EXTRACT_Writes_FROM_Node_INTEG = typename ExtractWrites<Node<Integrate>>::type;
    using Expected_CES_EXTRACT_Writes_INTEG = ResourcePack<es::Position>;
    static_assert(std::is_same_v<Expected_CES_EXTRACT_Writes_INTEG, CES_EXTRACT_Writes_FROM_Effects_INTEG>,
                  "Error in `ExtractWrites`");
    static_assert(std::is_same_v<Expected_CES_EXTRACT_Writes_INTEG, CES_EXTRACT_Writes_FROM_Node_INTEG>,
                  "Error in `ExtractWrites");

    using CES_HAS_RESOURCE_INTERSECTION_INTEG = HasReadWriteConflict<
        Effects<Integrate>,
        Effects<CollisionDetection>>; // Effects<Integrate>::reads && Effects<CollisionDetection>::writes -> Empty()
    static_assert(!CES_HAS_RESOURCE_INTERSECTION_INTEG::type::value,
                  "Error in `DetermineReadWrite`"); // expected == False

    // <Integrate>::writes::position and <CollisionDetection>::reads::position conflict
    static_assert(!CanRunTogether_v<CES_PROC_N_INTEG, CES_PROC_N_COLL>,
                  "Error in `CanRunTogether"); // expected == False

    using CES_APPEND_IF_CONFLICTS =
        typename AppendConflictRelation<RelationPack<>, CES_PROC_N_INTEG, CES_PROC_N_COLL>::type;
    // std::cerr << "CES_APPEND_IF_CONFLICTS Result = " << typeid(CES_APPEND_IF_CONFLICTS).name() << "\n";
    using CES_COLLECT_CONFLICTS_FOR_NODE_Res = typename CollectConflictRelations<RelationPack<>, CES_EXE_SET>::type;
    //std::cerr << "CES_COLLECT_CONFLICTS_FOR_NODE_Res = " << typeid(CES_COLLECT_CONFLICTS_FOR_NODE_Res).name() << "\n";
    using Exepected_CES_COLLECT_CONFLICTS_FOR_NODE_Res =
        RelationPack<Relation<Node<IntentSystem>, Node<Integrate>>, Relation<Node<Integrate>, Node<CollisionDetection>>,
                     Relation<Node<Integrate>, Node<CameraController>>, Relation<Node<Integrate>, Node<Renderer>>,
                     Relation<Node<CameraController>, Node<Renderer>>>;
    static_assert(std::is_same_v<Exepected_CES_COLLECT_CONFLICTS_FOR_NODE_Res, CES_COLLECT_CONFLICTS_FOR_NODE_Res>,
                  "Error in `CollectEffectRelations`");

    struct CES_TestTag
    {
    };
    using CES_SAFE_LAYERED_PLAN_RF =
        typename MakeSafeLayeredPlan<CES_TestTag, CES_EXE_SET, ResolverDirection::RootFirst>::type;
    // std::cerr <<  "CES_SAFE_LAYERED_PLAN_RF Result = " << typeid(CES_SAFE_LAYERED_PLAN).name() << "\n";
    using Expected_CES_SAFE_LAYERED_PLAN_RF = SafeLayeredPlan<
        LayerPack<NodePack<Node<IntentSystem>>, NodePack<Node<Integrate>>,
                  NodePack<Node<CollisionDetection>, Node<CameraController>>, NodePack<Node<Renderer>>>>;
    static_assert(std::is_same_v<Expected_CES_SAFE_LAYERED_PLAN_RF, CES_SAFE_LAYERED_PLAN_RF>,
                  "Error in `MakeSafelayeredPlan`");
    // ############# Deprecated #############
    // using CES_SAFE_LAYERED_PLAN_LF = typename MakeSafeLayeredPlan<CES_TestTag, CES_EXE_SET, ResolverDirection::LeafFirst>::type; LeafFirst is currently deprecated in Plans derived from EffectSystem.
    // using Expected_CES_SAFE_LAYERED_PLAN_LF = SafeLayeredPlan<LayerPack<NodePack<Node<CollisionDetection>,Node<Renderer>>,
    //                                                                     NodePack<Node<CameraController>>,
    //                                                                     NodePack<Node<Integrate>>,
    //                                                                     NodePack<Node<IntentSystem>>>>;
    // std::cerr <<  "CES_SAFE_LAYERED_PLAN_LF Result = " << typeid(CES_SAFE_LAYERED_PLAN_LF).name() << "\n";
    // static_assert(std::is_same_v<Expected_CES_SAFE_LAYERED_PLAN_LF, CES_SAFE_LAYERED_PLAN_LF >, "Error in `MakeSafelayeredPlan`");// Deprecated

    using CES_TEST_SAFE_LAYRES_PLAN_RF = typename MakeSafeLayeredPlan<CES_TestTag, CES_EXE_SET>::type;
    using Expected_CES_SAFE_LAYERED_PLAN_RF = SafeLayeredPlan<
        LayerPack<NodePack<Node<IntentSystem>>, NodePack<Node<Integrate>>,
                  NodePack<Node<CollisionDetection>, Node<CameraController>>, NodePack<Node<Renderer>>>>;

    struct TestContext
    {
    };
    TestContext test_ctx;
    std::cerr << "=== LINEAR ORDER EXECUTION ===\n";
    ExecutePlan<CES_TEST_SAFE_LAYRES_PLAN_RF, WarnExecutionConfig>::Run(test_ctx); // linear order

    AsyncTestContext async_test_ctx{};
    std::cerr << "\n=== PARALLEL EXECUTION  ===\n";
    try
    {
        ExecutePlanAsync<CES_TEST_SAFE_LAYRES_PLAN_RF, WarnExecutionConfig>::Run(async_test_ctx); // Parallel execution
    }
    catch (const std::exception& e)
    {
        std::cerr << "\n[caught] " << e.what() << "\n";
    }

    // std::cerr << "\n";
    // for(const auto& p : async_test_ctx.log)
    // {
    //     std::cerr << p << "\n";
    // }

    std::cerr << "\n ==== Test PlanStats ==== \n";
    using CES_STATS = PlanStats<CES_TEST_SAFE_LAYRES_PLAN_RF>;
    static_assert(CES_STATS::layer_count == 4, "error in PlanStats::layer_count");
    static_assert(CES_STATS::task_count == 5, "error in PlanStats::task_count");
    static_assert(CES_STATS::max_layer_width == 2, "error in PlanStats::max_layer_width");
    static_assert(CES_STATS::has_parallel_layer == true, "error in PlanStats::has_parallel_layer");
    static_assert(CES_STATS::is_fully_sequential == false, "error in PlanStats::is_fully_sequential");
    visualizer::PrintPlanStats<CES_TEST_SAFE_LAYRES_PLAN_RF>::Run();

    std::cerr << "\n ==== Test SafeLayeredPlan with runtime Thread Pool utility ==== \n";
    runtime::ThreadPool pool(CES_STATS::max_layer_width);
    // ExecutePlanWithPool<CES_TEST_SAFE_LAYRES_PLAN_RF, WarnExecutionConfig>::Run(pool,async_test_ctx);

    std::cerr << "\n ==== Test for uesr ThreadPool ====\n";
    es::MyThreadPool my_pool(CES_STATS::max_layer_width);
    // ExecutePlanWithPool<CES_TEST_SAFE_LAYRES_PLAN_RF, WarnExecutionConfig>::Run(my_pool,async_test_ctx);

    // ===================================== Visualization Tests =====================================
    std::cerr << "\n ==== Visualization Tests ==== \n";

    // SequentialPlan
    struct vs_sq_graph_tag
    {
    };
    using vs_sq_graph =
        Graph<vs_sq_graph_tag, Arc<Node<Position>, Node<Velocity>>, Arc<Node<Velocity>, Node<Acceleration>>>;
    using vs_sq_plan = typename MakeSequentialPlan<vs_sq_graph, ResolverDirection::RootFirst>::type;
    visualizer::PrintPlan<vs_sq_plan>::Run();

    std::cerr << "\n";
    // SafeLayeredPlan
    visualizer::PrintPlan<CES_TEST_SAFE_LAYRES_PLAN_RF>::Run();

    std::cerr << "\n";
    // HierarchicalPlan
    using vs_sq_graph_2 =
        Graph<vs_sq_graph_tag, Arc<Node<Acceleration>, Node<Velocity>>, Arc<Node<Velocity>, Node<Position>>>;
    struct vs_meta_graph_tag
    {
    };
    using vs_meta_graph = Graph<vs_meta_graph_tag, Arc<Node<vs_sq_graph>, Node<vs_sq_graph_2>>>;
    using vs_meta_plan = typename MakeHierarchicalPlan<vs_meta_graph, ResolverDirection::RootFirst,
                                                       ResolverDirection::RootFirst, PlanKind::Sequential>::type;
    visualizer::PrintPlan<vs_meta_plan>::Run();

    using foce_check_safe_layer_plan = ValidateSafeLayeredPlan<CES_TEST_SAFE_LAYRES_PLAN_RF>; // check conflicting
    static_assert(foce_check_safe_layer_plan::value,
                  "CES_TEST_SAFE_LAYRES_PLAN_RF contains conflicting nodes in the same layer");
    // using foce_check_safe_layer_plan = ValidateSafeLayerPlan<>;

    // =========================================== Validations ===========================================
    // for confict validation test
    std::cerr << "\n ============ Conflict Validation Testing ============\n";
    using InvalidSafeLayeredPlan = SafeLayeredPlan<LayerPack<NodePack<Node<Integrate>, Node<Renderer>>>>;
    static_assert(!ValidateSafeLayeredPlan<InvalidSafeLayeredPlan>::value,
                  "Invalid SafeLayeredPlan should fail validation");

    static_assert(ValidateExecutionSetUnique<CES_EXE_SET>::value,
                  "Since there are no duplicate nodes, detecting this here is a bug.");
    static_assert(!ValidateExecutionSetUnique<ExecutionSet<CES_PROC_N_CAMERA, CES_PROC_N_CAMERA>>::value,
                  "Node duplication must be detected.");

    static_assert(!ValidateExecutionSetEffects<ExecutionSet<DP_PROC_N_R>>::value,
                  "Duplicates in reads/writes must be detected.");
    static_assert(!ValidateExecutionSetEffects<ExecutionSet<DP_PROC_N_W>>::value,
                  "Duplicates in reads/writes must be detected.");

    static_assert(ValidateEffectsCompleteness<CES_EXE_SET>::value,
                  "There are nodes that do not define Effects<T>::reads/writes.");
    static_assert(
        !ValidateEffectsCompleteness<ExecutionSet<EC_PROC_N_R, EC_PROC_N_W>>::value,
        "This should result in an error because there is a Node that does not define Effects<T>::reads/writes.");

    static_assert(ValidatePlanNodesUnique<CES_TEST_SAFE_LAYRES_PLAN_RF>::value);

    using Invalid_Duplicated_Nodes_Plan =
        SafeLayeredPlan<LayerPack<NodePack<EC_PROC_N_R>, NodePack<EC_PROC_N_R, EC_PROC_N_W>>>;
    static_assert(!ValidatePlanNodesUnique<Invalid_Duplicated_Nodes_Plan>::value);
    // struct Test_Tag{};
    // using DP_Plan = typename MakeSafeLayeredPlan<Test_Tag, ExecutionSet<DP_PROC_N_W>>::type;// This should result in a compilation error.

    std::cerr << "\n ================ Plan Analyzer Test ================ \n";
    using analysis_result = typename AnalyzeParallelism<CES_TEST_SAFE_LAYRES_PLAN_RF>::type;
    static_assert(analysis_result::layer_count == 4);
    static_assert(analysis_result::sequential_layer_count == 3);
    static_assert(analysis_result::parallel_layer_count == 1);
    static_assert(analysis_result::parallelism_ratio == 0.25);

    visualizer::PrintParallelismAnalysis<CES_TEST_SAFE_LAYRES_PLAN_RF>::Run();

    std::cerr << "\n";
    using analysis_conflict_result = AnalyzeConflicts<CES_EXE_SET>::type::conflict_relations;
    using Expected_analysis_conflict_result =
        RelationPack<Relation<Node<IntentSystem>, Node<Integrate>>, Relation<Node<Integrate>, Node<CollisionDetection>>,
                     Relation<Node<Integrate>, Node<CameraController>>, Relation<Node<Integrate>, Node<Renderer>>,
                     Relation<Node<CameraController>, Node<Renderer>>>;
    static_assert(std::is_same_v<analysis_conflict_result, Expected_analysis_conflict_result>);
    visualizer::PrintConflictAnalysis<CES_EXE_SET>::Run();
    // visualizer::PrintConflictAnalysis<analysis_conflict_result>::Run();
    // visualizer::PrintConflictAnalysis<analysis_conflict>::Run();

    std::cerr << "\n";
    using CR_EXE_SET = ExecutionSet<CES_PROC_N_INTEG, CES_PROC_N_INTENT>;
    using collect_resources_res = typename CollectAllResources<ResourcePack<>, CR_EXE_SET>::type;
    using expected_collect_resources_res = ResourcePack<Intent, Velocity, Acceleration, Position, Input, Intent>;
    static_assert(std::is_same_v<collect_resources_res, expected_collect_resources_res>);
    // std::cerr << "collect_resources_res = " << typeid(collect_resources_res).name() << "\n";

    using ces_collect_readers_for_resource = typename CollectReadersForResource<NodePack<>, Position, CES_EXE_SET>::type;
    // std::cerr << "ces_collect_readers_for_resource_Res = " << typeid(ces_collect_readers_for_resource).name() << "\n";
    using expected_ces_collect_readers_for_resource = NodePack<CES_PROC_N_COLL, CES_PROC_N_CAMERA, CES_PROC_N_RENDER>;
    static_assert(std::is_same_v<ces_collect_readers_for_resource,expected_ces_collect_readers_for_resource>);
    using ces_collect_writers_for_resource = typename CollectWritersForResource<NodePack<>, Position, CES_EXE_SET>::type;
    using expected_ces_collect_writers_for_resource = NodePack<CES_PROC_N_INTEG>;
    static_assert(std::is_same_v<ces_collect_writers_for_resource, expected_ces_collect_writers_for_resource>);

    // using ces_build_resource_dependency = typename BuildResourceDependency<Position, CES_EXE_SET>::type;
    // std::cerr <<"ces_build_resource_dependency_Res = " << typeid(ces_build_resource_dependency).name() <<"\n";
    using CES_Analyze_Resource_Dependencies = typename AnalyzeResourceDependencies<CES_EXE_SET>::type;
    using Expected_CES_Analyze_Resource_Dependencies = ResourceDependencyAnalysisResult<
        ResourceDependency<Input,   NodePack<CES_PROC_N_INTENT>, NodePack<>>,
        ResourceDependency<Intent,  NodePack<CES_PROC_N_INTEG>, NodePack<CES_PROC_N_INTENT>>,
        ResourceDependency<Velocity,  NodePack<CES_PROC_N_INTEG>, NodePack<>>,
        ResourceDependency<Acceleration,  NodePack<CES_PROC_N_INTEG>, NodePack<>>,
        ResourceDependency<Position,  NodePack<CES_PROC_N_COLL,CES_PROC_N_CAMERA,CES_PROC_N_RENDER>, NodePack<CES_PROC_N_INTEG>>,
        ResourceDependency<Collision,  NodePack<CES_PROC_N_COLL>, NodePack<>>,
        ResourceDependency<CollisionMask,  NodePack<CES_PROC_N_COLL>, NodePack<>>,
        ResourceDependency<CollisionResult,  NodePack<>, NodePack<CES_PROC_N_COLL>>,
        ResourceDependency<Camera,  NodePack<CES_PROC_N_RENDER>, NodePack<CES_PROC_N_CAMERA>>
    >;
    static_assert(std::is_same_v<CES_Analyze_Resource_Dependencies, Expected_CES_Analyze_Resource_Dependencies>);
    visualizer::PrintResourceDependencyAnalysis<CES_EXE_SET>::Run();
    // std::cerr << "CES_Analyze_Resource_Dependency_Res = \n  " << typeid(CES_Analyze_Resource_Dependencies).name() <<"\n";
    // TODO:
    // 2. Critical path analysis:       Planの中で、並列化しても短縮できない最長経路を見る
    // 3. Resource dependency analysis: task ではなく resource 側から 見る
    // 5. Diagnostics:                  analysis結果をもとに「問題・改善候補」を出す層

    return 0;
}