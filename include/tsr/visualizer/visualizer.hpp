#pragma once

#include <iostream>
#include <string>
#include <type_traits>
#include "tsr/plan/plan.hpp"
#include "tsr/analysis/plan_stats.hpp"
#include "tsr/analysis/plan_analyzer.hpp"

namespace tsr::visualizer
{
    template <typename PlanT>
    struct PrintPlan;

    template <typename T>
    struct TypeName
    {
        static const char* Get()
        {
            return typeid(T).name();
        }
    };
    template <typename T>
    struct TypeName<Node<T>>
    {
        static const char* Get()
        {
            return typeid(T).name();
        }
    };

    template <typename NodeT>
    struct PrintNode;
    template <typename T>
    struct PrintNode<Node<T>>
    {

        static void Run(std::ostream& os, int indent = 0)
        {
            os << std::string(indent, ' ') << TypeName<T>::Get() << "\n";
        }
    };

    template <typename NodePackT>
    struct PrintNodePack;
    template <typename... NodeTs>
    struct PrintNodePack<NodePack<NodeTs...>>
    {
        static void Run(std::ostream& os, int indent = 0)
        {
            std::size_t index = 0;

            auto print_node = [&](auto tag)
            {
                using NodeT = typename decltype(tag)::type;

                os << std::string(indent, ' ') << "[" << index++ << "] ";

                PrintNode<NodeT>::Run(os, 0);
            };

            (print_node(std::type_identity<NodeTs>{}), ...);
        }
    };

    // ================= Sequential Plan =================
    template <typename... NodeTs>
    struct PrintPlan<SequentialPlan<NodePack<NodeTs...>>>
    {
        static void Run(std::ostream& os = std::cerr, int indent = 0)
        {
            os << std::string(indent, ' ') << "SequentialPlan\n";
            PrintNodePack<NodePack<NodeTs...>>::Run(os, indent + 2);
        }
    };

    // ================= Layered Plan =================
    template <typename LayerPackT>
    struct PrintLayerPack;
    template <typename... LayerTs>
    struct PrintLayerPack<LayerPack<LayerTs...>>
    {
        static void Run(std::ostream& os, int indent = 0)
        {
            std::size_t index = 0;

            auto print_layer = [&](auto tag)
            {
                using LayerT = typename decltype(tag)::type;

                os << std::string(indent, ' ') << "Layer " << index++ << "\n";

                PrintNodePack<LayerT>::Run(os, indent + 2);
            };

            (print_layer(std::type_identity<LayerTs>{}), ...);
        }
    };

    template <typename LayerPackT>
    struct PrintPlan<LayeredPlan<LayerPackT>>
    {
        static void Run(std::ostream& os = std::cerr, int indent = 0)
        {
            os << std::string(indent, ' ') << "LayeredPlan\n";
            PrintLayerPack<LayerPackT>::Run(os, indent + 2);
        }
    };

    // ================= SafeLayered Plan =================
    template <typename LayerPackT>
    struct PrintPlan<SafeLayeredPlan<LayerPackT>>
    {
        static void Run(std::ostream& os = std::cerr, int indent = 0)
        {
            os << std::string(indent, ' ') << "SafeLayeredPlan\n";
            PrintLayerPack<LayerPackT>::Run(os, indent + 2);
        }
    };

    template <typename SubPlanPackT>
    struct PrintSubPlan;
    template <typename GraphTag, typename GraphT, typename PlanT>
    struct PrintSubPlan<SubPlan<GraphTag, GraphT, PlanT>>
    {
        static void Run(std::ostream& os = std::cerr, int indent = 0)
        {
            os << std::string(indent, ' ') << "SubPlan: " << TypeName<GraphT>::Get() << "\n";

            PrintPlan<PlanT>::Run(os, indent + 2);
        }
    };

    template <typename SubPlanT>
    struct PrintSubPlanPack;
    template <typename... SubPlanTs>
    struct PrintSubPlanPack<SubPlanPack<SubPlanTs...>>
    {
        static void Run(std::ostream& os = std::cerr, int indent = 0)
        {
            (PrintSubPlan<SubPlanTs>::Run(os, indent), ...);
        }
    };

    template <typename MetaPlanT, typename SubPlanPackT>
    struct PrintPlan<HierarchicalPlan<MetaPlanT, SubPlanPackT>>
    {
        static void Run(std::ostream& os = std::cerr, int indent = 0)
        {
            os << std::string(indent, ' ') << "HierarchicalPlan\n";

            os << std::string(indent + 2, ' ') << "MetaPlan\n";

            PrintPlan<MetaPlanT>::Run(os, indent + 4);

            os << "\n";

            os << std::string(indent + 2, ' ') << "SubPlans\n";

            PrintSubPlanPack<SubPlanPackT>::Run(os, indent + 4);
        }
    };

    // PrintPlanStats<Plan>::Run();
    template <typename PlanT>
    struct PrintPlanStats
    {
        static void Run(std::ostream& os = std::cerr)
        {
            using stats = PlanStats<PlanT>;
            os << "PlanStats\n";

            os << "  layer_count: " << stats::layer_count << "\n";
            os << "  task_count: " << stats::task_count << "\n";
            os << "  max_layer_width: " << stats::max_layer_width << "\n";
            os << "  has_parallel_layer: " << std::boolalpha << stats::has_parallel_layer << "\n";
            os << "  is_fully_sequential: " << std::boolalpha << stats::is_fully_sequential << "\n";
        }
    };

    template <typename PlanT>
    struct PrintParallelismAnalysis
    {
        static void Run(std::ostream& os = std::cerr)
        {
            using result = typename AnalyzeParallelism<PlanT>::type;

            os << "ParallelismAnalysis\n";
            os << "  layer_count: " << result::layer_count << "\n";
            os << "  sequential_layer_count: " << result::sequential_layer_count << "\n";
            os << "  parallel_layer_count: " << result::parallel_layer_count << "\n";
            os << "  parallelism_ratio: " << result::parallelism_ratio << "\n";
        }
    };

    template <typename RelationT>
    struct PrintRelation;
    template <typename FirstT, typename SecondT>
    struct PrintRelation<Relation<Node<FirstT>, Node<SecondT>>>
    {
        static void Run(std::ostream& os = std::cerr)
        {
            os << "  " << typeid(FirstT).name() << " <-> " << typeid(SecondT).name() << "\n";
        }
    };

    template <typename RelationPackT>
    struct PrintRelationPack;
    template <typename... RelationTs>
    struct PrintRelationPack<RelationPack<RelationTs...>>
    {
        static void Run(std::ostream& os = std::cerr)
        {
            (PrintRelation<RelationTs>::Run(os), ...);
        }
    };

    template <typename T>
    struct PrintConflictAnalysis;

    template <typename... NodeTs>
    struct PrintConflictAnalysis<ExecutionSet<NodeTs...>>
    {
        static void Run(std::ostream& os = std::cerr)
        {
            using relations = typename AnalyzeConflicts<ExecutionSet<NodeTs...>>::type::conflict_relations;

            os << "ConflictAnalysis: \n";
            PrintRelationPack<relations>::Run(os);
        }
    };
    template <typename ConflictAnalysisResultT>
    struct PrintConflictAnalysis<ConflictAnalysisResult<ConflictAnalysisResultT>>
    {
        static void Run(std::ostream& os = std::cerr)
        {
            os << "ConflictAnalysis: \n";
            PrintRelationPack<ConflictAnalysisResultT>::Run(os);
        }
    };
    template <typename... RelationTs>
    struct PrintConflictAnalysis<RelationPack<RelationTs...>>
    {
        static void Run(std::ostream& os = std::cerr)
        {
            os << "ConflictAnalysis: \n";
            PrintRelationPack<RelationPack<RelationTs...>>::Run(os);
        }
    };

    template <typename... NodeTs>
    struct PrintNodes
    {
        static void Run(std::ostream& os = std::cerr)
        {
            ((os << "      " << TypeName<NodeTs>::Get() << "\n"), ...);
        }
    };
    template <typename... Ts>
    struct PrintNodes<Node<Ts>...>
    {
        static void Run(std::ostream& os = std::cerr)
        {
            ((os << "      " << TypeName<Ts>::Get() << "\n"), ...);
        }
    };

    template <typename T>
    struct PrintResourceDependency;
    template <typename ResourceT, typename... ReaderNodeTs, typename... WriterNodeTs>
    struct PrintResourceDependency<ResourceDependency<ResourceT, NodePack<ReaderNodeTs...>, NodePack<WriterNodeTs...>>>
    {
        static void Run(std::ostream& os = std::cerr)
        {
            os << "  Resource: " << typeid(ResourceT).name() << "\n";
            os << "    Readers: \n";
            PrintNodes<ReaderNodeTs...>::Run(os);
            os << "    Writers: \n";
            PrintNodes<WriterNodeTs...>::Run(os);
            os << "\n";
        }
    };

    template <typename DependencyPackT>
    struct PrintAllResourceDependency;
    template <typename... DependenciesT>
    struct PrintAllResourceDependency<ResourceDependencyAnalysisResult<DependenciesT...>>
    {
        static void Run(std::ostream& os = std::cerr)
        {
            ((PrintResourceDependency<DependenciesT>::Run(os)), ...);
        }
    };

    template <typename T>
    struct PrintResourceDependencyAnalysis;
    template <typename... NodeTs>
    struct PrintResourceDependencyAnalysis<ExecutionSet<NodeTs...>>
    {
        static void Run(std::ostream& os = std::cerr)
        {
            using dependencies = typename AnalyzeResourceDependencies<ExecutionSet<NodeTs...>>::type;

            os << "ResourceDependencyAnalysis: \n";
            PrintAllResourceDependency<dependencies>::Run(os);
        }
    };
    template <typename... ResourceDependencies>
    struct PrintResourceDependencyAnalysis<ResourceDependencyAnalysisResult<ResourceDependencies...>>
    {
        static void Run(std::ostream& os = std::cerr)
        {
            os << "ResourceDependencyAnalysis: \n";
            PrintAllResourceDependency<ResourceDependencyAnalysisResult<ResourceDependencies...>>::Run(os);
        }
    };

    template <typename T>
    struct PrintEstimatedCriticalPathAnalysis;
    template <std::size_t Cost, typename... NodeTs>
    struct PrintEstimatedCriticalPathAnalysis<EstimatedCriticalPathAnalysisResult<Cost, NodePack<NodeTs...>>>
    {
        static void Run(std::ostream& os = std::cerr)
        {
            os << "EstimatedCriticalPathAnalysis:\n";

            os << "  critical_path_cost: " << Cost << "\n";

            os << "  critical_path:\n";

            PrintNodePack<NodePack<NodeTs...>>::Run(os, 4);
        }
    };
    template <typename PlanT>
    struct PrintEstimatedCriticalPathAnalysis
    {
        static void Run(std::ostream& os = std::cerr)
        {
            using result = typename AnalyzeEstimatedCriticalPath<PlanT>::type;

            PrintEstimatedCriticalPathAnalysis<result>::Run(os);
        }
    };

    template <typename PlanT>
    struct PrintSafeLayeredPlanDiagnostics
    {
        static void Run(std::ostream& os = std::cerr)
        {
            using stats = PlanStats<PlanT>;
            using parallelism = typename AnalyzeParallelism<PlanT>::type;
            using estimated_cp = typename AnalyzeEstimatedCriticalPath<PlanT>::type;

            os << "Diagnostics:\n";

            os << "  layer_count: " << stats::layer_count << "\n";
            os << "  task_count: " << stats::task_count << "\n";

            os << "  max_layer_width: " << stats::max_layer_width << "\n";

            os << "  parallel_layer_count: " << parallelism::parallel_layer_count << "\n";

            os << "  sequential_layer_count: " << parallelism::sequential_layer_count << "\n";

            os << "  parallelism_ratio: " << parallelism::parallelism_ratio << "\n";

            os << "  estimated_critical_path_cost: " << estimated_cp::critical_path_cost << "\n";

            if constexpr (stats::is_fully_sequential)
            {
                os << "  note: plan is fully sequential\n";
            }

            if constexpr (stats::has_parallel_layer)
            {
                os << "  note: plan contains parallelizable layers\n";
            }

            os << "\n  estimated_critical_path:\n";
            PrintNodePack<typename estimated_cp::critical_path>::Run(os, 4);
        }
    };
} // namespace tsr::visualizer