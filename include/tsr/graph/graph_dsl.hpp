/*
Defines how to write a DSL.
*/

#pragma once

#include <type_traits>

namespace tsr
{

    inline constexpr int indent_width = 2;

    // Node<T>      // 外部型を包む
    // Arc<R, B...> // R <- B... の関係
    // Graph<Tag, Elements...>
    // ============================================================== DESCRPT ==============================================================
    template <typename T>
    struct Node
    {
        using basic_type = T;
    };

    // ============================================ Validations ============================================
    // *********************** type traits ***********************
    template <typename T>
    struct IsNode : std::false_type
    {
    };
    template <typename T>
    struct IsNode<Node<T>> : std::true_type
    {
    };
    template <typename T>
    concept NodeType = IsNode<T>::value;

    // Check Duplication
    template <typename T, typename... Ts>
    struct Contains : std::bool_constant<(std::is_same_v<T, Ts> || ...)>
    {
    };
    template <typename... Ts>
    struct IsUnique : std::true_type
    {
    };
    template <typename T, typename... Rest>
    struct IsUnique<T, Rest...> : std::bool_constant<!Contains<T, Rest...>::value && IsUnique<Rest...>::value>
    {
    };
    // =====================================================================================================
    // ***********************************************************

    // ###################### TYPE ######################
    template <NodeType... Nodes>
    struct NodePack
    {
    };

    template <typename... BranchTs>
    struct ElementPack
    {
    };

    // ====================== fwd =========================
    template <typename T>
    struct IsElement;
    // fwd
    // ====================================================

    template <NodeType RootNodeT, typename... BranchTs> // Arc<Node, Branch...>//
    struct Arc
    {
        static_assert((IsElement<BranchTs>::value && ...), "Arc branches must be Node<T> or valid Arc<Node<T>, ...>");

        static_assert(IsUnique<BranchTs...>::value, "Arc elements must be unique");
        using root_node = RootNodeT;
        using branches = ElementPack<BranchTs...>;
    };
    // ##################################################

    // ============================================ Validations ============================================
    template <typename T>
    struct IsElement : std::false_type
    {
    };
    template <typename T>
    struct IsElement<Node<T>> : std::true_type
    {
    };
    // template<PureNode RootNodeT, typename... BranchTs>
    // struct IsArcBranch<Arc<RootNodeT, BranchTs...>> : std::bool_constant<(IsArcBranch<BranchTs>::value&&...)>{};
    template <NodeType RootNodeT, typename... BranchTs>
    struct IsElement<Arc<RootNodeT, BranchTs...>> : std::true_type
    {
    };
    template <typename T>
    concept ElementType = IsElement<T>::value; // // Node<T> or Arc<...>

    // // validate
    template <typename T>
    struct IsValidElement : std::false_type
    {
    };
    template <typename T>
    struct IsValidElement<Node<T>> : std::true_type
    {
    };
    template <NodeType RootNodeT, typename... BranchTs>
    struct IsValidElement<Arc<RootNodeT, BranchTs...>> : std::bool_constant<(IsValidElement<BranchTs>::value && ...)>
    {
    }; // check if BranchTs are Node<T>
    // // =====================================================================================================
    // // ***********************************************************

    // ###################### TYPE ######################
    template <typename Tag, ElementType... ElementTs>
    struct Graph
    {
        static_assert((IsElement<ElementTs>::value && ...), "Graph elements must be Node<T> or Arc<Node<T>, ...>");
        static_assert((IsValidElement<ElementTs>::value && ...),
                      "Graph elements must be valid Node<T> or valid Arc<Node<T>, ...>");

        using tag_type = Tag;
        using element_types = ElementPack<ElementTs...>;
    };
    // ##################################################
    // ==========================================================================================================================================================================================================================================================================

} // namespace tsr