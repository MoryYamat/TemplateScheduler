# Type-level semantic compiler infrastructure
A non-invasive type-level semantic IR framework for projecting existing C++ types into analyzable compile-time representations.

existing types as semantics

**semantic graph verifier / planner**

## Key Idea
```
Existing Types
    ↓
Structural Projection
    ↓
Canonical Graph IR
    ↓
Semantic Interpretation Passes
    ↓
Artifacts / Plans / Validation / Execution
```

### properties
```
Graph:
    purely structural

IR:
    normalized structure

Interpretation:
    edge meaning

Plan:
    execution / scheduling representation

Executor:
    runtime realization
```

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

### projection:  
External systems are integrated through adapters/projections,  
rather than requiring ownership or inheritance.

### Runtime separation
```
Compile-time:
  dependency analysis
  plan synthesis
  scheduling metadata

Runtime:
  task execution
  thread reuse
  worker scheduling
```

## Template Execution Order Resolver (Scheduler)
- resolve compile time or Only once at runtime

TemplateScheduler is a working name.
The project is not limited to scheduling.
Its core is a non-invasive type-level directed graph DSL and compiler pipeline for generating validated execution plans.

## Structure
**The same Graph can be interpreted in multiple ways.**

**A Graph is intentionally strucutral and non-invasive.**
It does not force a single meaning such as dependency, ownership, ontology, or execution order.

Different compiler  passes may interpret the same normalized IR differently and produce different artifacts:
- Ontology
- Execution plan
- Visualization graph
- Validation graph
- Ownership graph

```
Graph:
  purely structural
IR:
  normalized structure
Interpretation:
  edge meaning
Plan:
  execution/scheduling representation
Executor:
  runtime realization
```

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
LayeredPlan<LayerPack<NodePack<...>,...>>
SafeLayeredPlan<LayerPack<NodePack<...>>>
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

```cpp
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
```cpp
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
- DefaultExecutionConfig: DefaultExecutionConfig: fail at compile time if no executable form is found
- WarnExecutionConfig: Warning (standard input/output) if `UserType<T>::Run()` is not defined
- SkipExecutionConfig: Skip execution (do not execute) regardless of whether `UserType<T>::Run()` is defined or not


### Parallel Execution
- Parallel executor assumes Effects<T> accurately describes all shared mutable state access.

Parallel safety contract:
- Effects<T> may over-approximate, but must not under-approximate,
- the shared mutable state accessed by T.


## Create a conflict-safe layered execution plan from an access effect system
```cpp
using exe_set = ExecutionSet<Node<user_type_A>,Node<user_type_B>,Node<user_type_C>,...>;

using plan = typename MakeSafeLayeredPlan<exe_set, WarnExecutionConfig>::type;
using stats = PlanStats<plan>;

UserContext user_ctx;
UserThreadPool user_pool(stats::max_layer_width);
ExecutePlanWithPool<plan, WarnExecutionConfig>::Run(user_pool,user_ctx);
```

If Submit is an alias, the following specialization is required
```cpp
template<>
struct PoolAdapter<UserThreadPool>
{
    template<typename F>
    static auto Submit(UserThreadPool& pool, F&& f)
    {
        return pool.enqueue(std::forward<F>(f));
    }
};
```


### `PlanStats`
a LayeredPlan synthesized from execution conflict analysis
```
using stats = PlanStats<MyPlan>;
stats::layer_count
stats::task_count
stats::max_layer_width
stats::has_parallel_layer
stats::is_fully_sequential
```

## Visualize
You can display Plan information using the following function.
```cpp
// Output the structure of Pan as a string to standard input/output.
visualizer::PrintPlan<Plan>::Run();

// Output structural statistics information of the plan to standard input/output.
visualizer::PrintPlanStats<Plan>::Run();

// Output parallel execution plan statistics to standard input/output.
visualizer::PrintParallelismAnalysis<Plan>::Run();
```

It can be used with all plans.
- `SequentialPlan`
- `LayeredPlan`
- `SafeLayeredPlan`
- `HierarchicalPlan`

The default argument for `PrintPlan::Run()` is `std::cerr`. `std::cout` and `std::clog` can also be used.


## Validations
hard validation


## Analysis
```
// Calculate the number of linearly ordered layers, the number of parallelized layers, and the parallelization ratio for the plan.
using analysis_result = typename AnalyzeParallelism<Plan>::type;

// These results can be displayed all at once using the following function.
visualizer::PrintParallelismAnalysis<Plan>::Run();
```

