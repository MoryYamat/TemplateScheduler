#pragma once

#include <iostream>
#include <cstdint>

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
    // *********************** type traits ***********************
    // 1. Relation<Root, Branch>// primitive
    template <NodeType RootNodeT, NodeType Branch>
    struct Relation
    {
        static void Print()
        {
            std::cerr << "Root_Type = " << typeid(RootNodeT).name() << "/ Child_Type = " << typeid(Branch).name()
                      << "\n";
        }
    };

    template <typename... Relations>
    struct RelationPack
    {
    };

    // 2. BranchRoot<T>// to extract Arc<Node<RootNode>, Node<BranchTs>...> and Arc<Node<RootNode>, Arc<RootNode,BranchTs...>>
    template <typename T>
    struct ElementRoot;
    template <typename T>
    struct ElementRoot<Node<T>>
    {
        using type = Node<T>; // ElementRoot<Node<T>>::type
    };
    template <NodeType RootNodeT, typename... BranchTs>
    struct ElementRoot<Arc<RootNodeT, BranchTs...>>
    {
        using type = RootNodeT; // ElementRoot<Arc<RootNodeT, BranchTs...>::type == Node<RootNodeT>

        // delete
        // using branchTs =
        //     RelationPack<BranchTs...>; // ElementRoot<Arc<RootNodeT, BranchTs...>::branchTs == ElementPack<BranchTs...>
    };

    // 4. ElementPack の concat
    template <typename... Packs>
    struct Concat;
    template <>
    struct Concat<>
    {
        using type = RelationPack<>;
    };
    template <typename... Ts>
    struct Concat<RelationPack<Ts...>>
    {
        using type = RelationPack<Ts...>;
    };
    template <typename... Ts, typename... Us, typename... Rest>
    struct Concat<RelationPack<Ts...>, RelationPack<Us...>, Rest...>
    {
        using type = typename Concat<RelationPack<Ts..., Us...>, Rest...>::type;
    };

    // ToRelations
    template <typename T>
    struct ToRelations;
    template <typename T>
    struct ToRelations<Node<T>>
    {
        using type = RelationPack<>;
    };
    template <NodeType RootNodeT, typename... BranchTs>
    struct ToRelations<Arc<RootNodeT, BranchTs...>>
    {
        using direct_type = RelationPack<Relation<RootNodeT, typename ElementRoot<BranchTs>::type>...>; // direct

        using nested_type = typename Concat<typename ToRelations<BranchTs>::type...>::type; // nested

        using type = typename Concat<direct_type, nested_type>::type; // concat

        static void Print()
        {
            std::cerr << typeid(type).name() << "\n";
        }
    };

    // 5. IsUnique<flattened relations...>
    template <typename Pack>
    struct IsUniqueRelations : std::false_type
    {
    };
    template <typename... Relations>
    struct IsUniqueRelations<RelationPack<Relations...>> : IsUnique<Relations...>
    {
    };

    // ============== Bidirectional edge detection ==============
    template <typename A, typename... Relations>
    struct ContainsReverse;
    template <typename A, typename B, typename... Relations>
    struct ContainsReverse<Relation<A, B>, Relations...>
        : std::bool_constant<(std::is_same_v<Relation<B, A>, Relations> || ...)>
    {
    };
    template <typename... Relations>
    struct HasBidirectionalRelation : std::false_type
    {
    };
    template <typename R, typename... Rest>
    struct HasBidirectionalRelation<R, Rest...>
        : std::bool_constant<(ContainsReverse<R, Rest...>::value || HasBidirectionalRelation<Rest...>::value)>
    {
    };
    // for pack
    template <typename Pack>
    struct HasBidirectionalRelationInPack : std::false_type
    {
    };
    template <typename... Relations>
    struct HasBidirectionalRelationInPack<RelationPack<Relations...>> : HasBidirectionalRelation<Relations...>
    {
    };

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

    // validate
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
    // =====================================================================================================
    // ***********************************************************

    // ###################### TYPE ######################
    template <typename Tag, ElementType... ElementTs>
    struct Graph
    {
        static_assert((IsElement<ElementTs>::value && ...), "Graph elements must be Node<T> or Arc<Node<T>, ...>");

        static_assert((IsValidElement<ElementTs>::value && ...),
                      "Graph elements must be valid Node<T> or valid Arc<Node<T>, ...>");

        using relations = typename Concat<typename ToRelations<ElementTs>::type...>::type;
        // ++++++++  Check for duplicates in Relation<A,B> ++++++++ 
        static_assert(IsUniqueRelations<relations>::value,
                      "Duplicated relations detected"); 
        // ++++++++ Check for bidirectional edges ++++++++ 
        static_assert(!HasBidirectionalRelationInPack<relations>::value, "Bidirectional relations are not allowed");
        // ++++++++ Check for circular node ++++++++
        // static_assert(!Has)

        static void Print()
        {
            std::cerr << typeid(relations).name() << "\n";
        }

        using tag_type = Tag;
        using element_types = ElementPack<ElementTs...>;
    };
    // ##################################################
    // ==========================================================================================================================================================================================================================================================================

    // ============================================================== COMPILER ==============================================================

    // =================== Scanning policy ========================
    // @The resolver's responsibility is to specify the traversal semantics.()
    enum class ResolverDirection : std::uint32_t
    {
        RootFirst,
        LeafFirst
    };

    // helpers
    template <ResolverDirection Direction>
    inline constexpr void PrintResolverDirection()
    {
        if constexpr (Direction == ResolverDirection::RootFirst)
        {
            std::cerr << "Root First\n";
        }
        else
        {
            std::cerr << "Leaf First\n";
        }
    }

    template <typename Tag>
    struct TagResolver
    {
        static void Resolve()
        {
            std::cerr << "Graph Type = " << typeid(Tag).name() << "\n";
        }
    };

    void indent(int indent = 0)
    {
        for (int i = 0; i < indent; i++)
            std::cerr << "-";
    }

    // ======================== Resolver(Specify Scanning policy) ==========================
    // TEST
    // UNIT
    template <typename T, ResolverDirection Direction = ResolverDirection::RootFirst>
    struct NodeResolver
    {
        static void Resolve(int i)
        {
            indent(i);
            std::cerr << "Type = " << typeid(T).name() << "\n";
        }

        static void Resolve()
        {
            std::cerr << "Type = " << typeid(T).name() << "\n";
        }
    };
    template <typename T, ResolverDirection Direction>
    struct NodeResolver<Node<T>, Direction>
    {
        static void Resolve(int i)
        {
            NodeResolver<T, Direction>::Resolve(i);
        }

        static void Resolve()
        {
            NodeResolver<T, Direction>::Resolve();
        }
    };
    template <typename Root, typename... ElementTs, ResolverDirection Direction>
    struct NodeResolver<Arc<Root, ElementTs...>, Direction>
    {
        static void Resolve(int i)
        {
            if constexpr (Direction == ResolverDirection::RootFirst)
            {
                NodeResolver<Root, ResolverDirection::RootFirst>::Resolve(i);
                ((NodeResolver<ElementTs, ResolverDirection::RootFirst>::Resolve(i + 4)), ...);
            }
            else
            {
                ((NodeResolver<ElementTs, ResolverDirection::LeafFirst>::Resolve(i + 4)), ...);
                NodeResolver<Root, ResolverDirection::LeafFirst>::Resolve(i);
            }
        }

        static void Resolve()
        {
            if constexpr (Direction == ResolverDirection::RootFirst)
            {
                NodeResolver<Root, ResolverDirection::RootFirst>::Resolve();
                ((NodeResolver<ElementTs, ResolverDirection::RootFirst>::Resolve()), ...);
            }
            else
            {
                ((NodeResolver<ElementTs, ResolverDirection::LeafFirst>::Resolve()), ...);
                NodeResolver<Root, ResolverDirection::LeafFirst>::Resolve();
            }
        }
    };

    template <typename Tag, typename... ElementTs, ResolverDirection Direction>
    struct NodeResolver<Graph<Tag, ElementTs...>, Direction>
    {
        static void Resolve(int i)
        {
            TagResolver<Tag>::Resolve();
            ((NodeResolver<ElementTs, Direction>::Resolve(i)), ...);
        }

        static void Resolve()
        {
            TagResolver<Tag>::Resolve();
            ((NodeResolver<ElementTs, Direction>::Resolve()), ...);
        }
    };
    // UNIT

    template <typename Graph, ResolverDirection Direction = ResolverDirection::RootFirst>
    struct ResolveOrder;

    template <typename Tag, ElementType... ElementTs, ResolverDirection Direction>
    struct ResolveOrder<Graph<Tag, ElementTs...>, Direction>
    {
        static void Resolve(int indent)
        {
            PrintResolverDirection<Direction>();
            NodeResolver<Graph<Tag, ElementTs...>, Direction>::Resolve(0);
        }

        static void Resolve()
        {
            PrintResolverDirection<Direction>();
            NodeResolver<Graph<Tag, ElementTs...>, Direction>::Resolve();
        }
    };
    // ======================================================================================================================================

    // ==================== Topological Sort ======================
    template<typename Pack, typename T>
    struct ContainsInPack;
    template<typename...Nodes, typename NodeT>
    struct ContainsInPack<NodePack<Nodes...>, NodeT> : Contains<NodeT, Nodes...>{};

    template<typename Pack, typename NodeT>
    struct AppendUnique;
    template<typename... NodeTs, typename NodeT>
    struct AppendUnique<NodePack<NodeTs...>, NodeT>
    {
        using type = std::conditional_t<
                    ContainsInPack<NodePack<NodeTs...>, NodeT>::value,
                    NodePack<NodeTs...>,
                    NodePack<NodeTs..., NodeT>
                    >;
    };
    
    template<typename NodePack, typename RelationT>
    struct AppendRelationNodes;
    template<typename NodePack, typename A, typename B>
    struct AppendRelationNodes<NodePack, Relation<A,B>>
    {
        using with_a = typename AppendUnique<NodePack, A>::type;
        using type = typename AppendUnique<with_a, B>::type;
    };

    template<typename NodePack, typename RelationPackT>
    struct CollectNodesImpl;
    template<typename Pack>
    struct CollectNodesImpl<Pack, RelationPack<>>
    {
        using type = Pack;
    };
    template<typename NodePack, typename R, typename... Rest>
    struct CollectNodesImpl<NodePack, RelationPack<R, Rest...>>
    {
        using next = typename AppendRelationNodes<NodePack, R>::type;
        using type = typename CollectNodesImpl<next, RelationPack<Rest...>>::type;
    };

    template<typename RelationPackT>
    struct CollectNodes;
    template<typename... Relations>
    struct CollectNodes<RelationPack<Relations...>>
    {
        using type = typename CollectNodesImpl<NodePack<>, RelationPack<Relations...>>::type;
    };

    // 
    template<typename NodeT,typename Relation, ResolverDirection Direction = ResolverDirection::RootFirst>
    struct HasPredecessorImpl : std::false_type{};
    template<typename NodeT, NodeType A, NodeType B>
    struct HasPredecessorImpl<NodeT, Relation<A, B>, ResolverDirection::RootFirst> :
        std::bool_constant<(std::is_same_v<NodeT, B>)>{};// check if a Predecessor exists in NodeT.
    template<typename NodeT, NodeType A, NodeType B>
    struct HasPredecessorImpl<NodeT, Relation<A, B>, ResolverDirection::LeafFirst> :
        std::bool_constant<(std::is_same_v<NodeT, A>)>{};// check if a Predecessor exists in NodeT(Leaf First).
    
    template<typename NodeT, typename RelationPackT, ResolverDirection Direction = ResolverDirection::RootFirst>
    struct HasPredecessor;
    template<typename NodeT, ResolverDirection Direction, typename... Relations>
    struct HasPredecessor<NodeT, RelationPack<Relations...>, Direction>
        : std::bool_constant<(HasPredecessorImpl<NodeT, Relations, Direction>::value || ... )>{};

    // Collect nodes that do not have a preceding node
    template<typename ResultPack, typename NodeT, typename RelationPackT, ResolverDirection Direction>
    struct AppendIfNoPredecessorNodes;

    template<typename ResultPack, NodeType NodeT, typename RelationPackT, ResolverDirection Direction>
    struct AppendIfNoPredecessorNodes<ResultPack, NodeT, RelationPackT, Direction>
    {
        using type = std::conditional_t<HasPredecessor<NodeT, RelationPackT, Direction>::value, ResultPack, typename AppendUnique<ResultPack, NodeT>::type>;
    };

    template<typename ResultPackT, typename NodePackT, typename RelationPackT, ResolverDirection Direction>
    struct CollectReadyNodesImpl
    {

    };
    template<typename ResultPackT,typename RelationPackT, ResolverDirection Direction>
    struct CollectReadyNodesImpl<ResultPackT, NodePack<>, RelationPackT, Direction>
    {
        using type = ResultPackT;
    };
    template<typename ResultPackT, typename HeadNodeT, typename... RestNodeTs, typename RelationPackT, ResolverDirection Direction>
    struct CollectReadyNodesImpl<ResultPackT, NodePack<HeadNodeT, RestNodeTs...>, RelationPackT, Direction>
    {
        using next_result = typename AppendIfNoPredecessorNodes<ResultPackT, HeadNodeT, RelationPackT, Direction>::type;
        using type = typename CollectReadyNodesImpl<next_result, NodePack<RestNodeTs...>, RelationPackT, Direction>::type;
    };
    
    template<typename NodePackT, typename RelationPackT, ResolverDirection Direction>
    struct CollectReadyNodes;
    template<typename... NodeTs, typename RelationPackT, ResolverDirection Direction>
    struct CollectReadyNodes<NodePack<NodeTs...>, RelationPackT, Direction>
    {
        using type = typename CollectReadyNodesImpl<NodePack<>, NodePack<NodeTs...>, RelationPackT, Direction>::type;
    };


    template<typename RelationPack, ResolverDirection Direction = ResolverDirection::RootFirst>
    struct TopologicalSort;

    // // root first
    template<typename...  Relations>
    struct TopologicalSort<RelationPack<Relations...>, ResolverDirection::RootFirst>
    {
        using type = RelationPack<Relations...>;

        
        static void Print()
        {
            ((std::cerr << typeid(Relations).name() << "\n"),...);
            // AppendUnique<Relations...>::Print();// 

        }
    };

    // leaf first
    template<typename...  Relations>
    struct TopologicalSort<RelationPack<Relations...>, ResolverDirection::LeafFirst>
    {        
        using type = RelationPack<Relations...>;

        
        static void Print()
        {
            ((std::cerr << typeid(Relations).name() << "\n"),...);
        }
    };

    // Conversion from a partial order to a linear order
    template<typename GraphT, ResolverDirection Direction = ResolverDirection::RootFirst>
    struct TopologicalOrder
    {
        using relations = typename GraphT::relations;
        using nodes = typename CollectNodes<relations>::type;
        // using type = NodePack<TopologicalSort<relations, Direction>>;

        static void Print()
        {
            std::cerr << typeid(nodes).name() << "\n\n";
            // TopologicalSort<relations, Direction>::Print();

        }
    };



    // ============================================================

    // ==================== TOPOLOGICAL SORT ======================
    // using Order = ResolverOrder<Graph, DependencFirst>::type;

    // ============================================================

    // ====================== REVERSE ORDER =======================

    // ============================================================

    // ===================== CYCLE DETECTION ======================

    // ============================================================

    // ====================== PRALLEL LAYER =======================

    // ============================================================
} // namespace tsr