# TShceduler
existing types as semantics

**semantic graph verifier / planner**

## Template Execution Order Resolver (Scheduler)
- resolve compile time or Only once at runtime

TemplateScheduler is a working name.
The project is not limited to scheduling.
Its core is a non-invasive type-level directed graph DSL and compiler pipeline for generating validated execution plans.

## 方針
- 1. 対象型に基底クラスを要求しない
- 2. 対象型にmember function名を要求しない
- 3. 対象型にmacroを埋め込まない
- 4. traits/specialization/adapterで外側から接続できる
- 5. 実行方法も Executor<T> で外部定義できる

## Policy
- 1. Do not require a base class for the target type.
- 2. Do not require a member function name for the target type.
- 3. Do not embed macros in the target type.
- 4. Connect from the outside using traits/specialization/adapter.
- 5. Execution method can also be defined externally using Executor<T>.


## Core Semantic Assumptions

- `Node<T>` is the canonical semantic unit.
- All graph elements are represented through `Node<>`
-The system is non-invasive:  
user-defined types are never modified or inherited from frame work types.
- `Arc<Parent, Children...>` represents asymmetric relations.
- The meaning of an edge is intentionally abstract:  
dependency execution order, ownership, precedence, etc.
- Immediate bidirectional edges are invalid
- Cyclic relations are currently rejected, but future policies may allow controlled cycle resolution.
- `Graph<Tag, ...>` represents a collection of semantic relations.
- Multiple disconnected roots are allowed.
- `Node<Graph<...>>` is the canonical form for hierarchical graphs / meta-graphs.

## Canonical Forms

### execution plan (Sequential or Layered)
```
SequentialPlan<NodePack<...>>
LayeredPlan<NodePack<...>>
```

### Hierarchical execution plan
```
HierarchicalPlan<
    MetaPlan,
    SubPlanPack<SubPlan<Graph<>, (Sequential/Layered)Plan<...>>,...>>
```

### Meta-graph node
```
Node<Graph<...>>
```

## Example

```
using A = Graph<...>;
using B = Graph<...>;

using Meta =
    Graph<
        MetaTag,
        Arc<
            Node<A>,
            Node<B>
        >
    >;

using Plan =
    MakeHierarchicalPlan<
        Meta,
        ResolverDirection::RootFirst,                       // meta graph direction
        ResolverDirection::LeafFirst                        // sub graph default direction

        GraphDirection<A, ResolverDirection::RootFirst>     // direction override
    >::type;
```


### Execution Definition
- User-specific functions can be defined in one of the following formats 
- if multiple exist, partial specializations are used, with the one for which a Context exists taking precedence.
```
    struct TestContext
    {
        std::vector<std::string> log;
    };

    struct A{};
    struct B{};
    struct C
    {
        static void Run(TestContext& ctx)
        {
            ctx.log.push_back("static_context");
        }
    };
    struct D
    {
        inline static bool called = false;

        static void Run()
        {
            called = true;
        }
    };


    // ============== Defining a user `Run` function through partial specialization of `Executor` ============== 
    // By setting ExecutionConfing in ConfigT, you can select how to run the program (Assert/Warn/Skip).
    template<>
    struct Executor<A>
    {
        template<typename ConfigT, typename Context>
        static void Run(Context& ctx)
        {
            ctx.log.push_back("executor_config");
        }
    };

    template<>
    struct Executor<B>
    {
        template<typename Context>
        static void Run(Context& ctx)
        {
            ctx.log.push_back("executor_plain");
        }
    };
```

#### Missing Executor Policy
- DefaultExecutionConfig: `static_assert(UserType<T>::Run())`
- WarnExecutionConfig: Warning (standard input/output) if `UserType<T>::Run()` is not defined
- SkipExecutionConfig: Skip execution (do not execute) regardless of whether `UserType<T>::Run()` is defined or not