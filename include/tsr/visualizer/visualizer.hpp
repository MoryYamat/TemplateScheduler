#pragma once

#include <iostream>
#include <string>
#include <type_traits>
#include "tsr/plan/plan.hpp"

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
} // namespace tsr::visualizer
// sequential plan

// Sequential Plan
// [0]A
// [1]B
// [2]C
// ....

// Layered Plan
// Layer 0:
//      A
//      B
// Layer 1:
//      C
// ...

// SafeLayeredPlan
// SafeLayer  0
//      A
//      B
// barrier
// SafeLayer  1
//      C

// HierarchicalPlan
// MetaPlan:
//      GraphA
//      GraphB
// ...
// SubPlan
//      Sequential: GraphA
//          A0
//          A1
//      Sequential: GraphB
//          B0
//          ...
