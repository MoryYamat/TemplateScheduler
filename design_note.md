Input or available information for this library
- View <Component> 
     - selected 
     - required
     - excluded

Output


- Execution Order
e.g.
- UpdateIntent();
     - UpdateMovementIntent();  
- UpdateMovement();  
     - UpdateMovementAcceleration();  
     - UpdateMovementSpeed();  
     - UpdateEntityPosition();  

[hypothesis]:  
[- hierarchical order(structure)]

- there are two type of processes: that can be executed in parallel and that need to be executed in stages

Is that solvable from the input?
- partially yes

What determines the execution order?
- **Meta hierarcy of the Components** (A component inherently contains a semantic structure)

e.g.
- VelocityComponent assumes the existence of PositionComponent. (Mathematically and logically (Newtonian mechanics))

this relation need to be represented explicitly.

struct Vel{Vec3f v;}; struct Pos{Vec3f v;};

                                                                  dependency representation: supplier <- client  (The client is dependent on supplier)

ComponentMetaHierarchy<Position>: Pos <- Vel <- Acc <- ...  
ComponentMetaHierarchy<Collision>: ComponentMetaHierarchy<Position> (or Pos) <- Hitbox <- mask <- ...  

[hypothesis]:  
[The processing order for components must be from the client side of the dependency upwards]  
-> Therefore, wouldn't it be better to determine the execution order from this ComponentMetaHierarchy graph?

[Main points]: In other words, if we can **construct a partially ordered structure**, we can say that we're 50% done.

[Expectation]: Since components are semantics, and semantics can be determined at programming time, this hierarchical structure should be constructible at compilation time.

 [Question]: Does a circular structure exist in a component? If so, what should be done about it?

[solution]: How about having the system itself handle its own dependency graph position?

is this it ?.
```
template<typename Order = DependencyGraph>
void UpdateAllFunction<Order>(Order& order)
{
     for( (Solve the Order from the bottom up)... )
     {
         Update<Order>(world&);
     }
}
```

## ComponentHierarchy construction system

- Input
- Local (declarative) dependencies from each component

- Interface

Constructor
```
// ConstructComponentHierarchy.h
// #include <ComponentHierarchy>
// #include <Components>
// namespace hier
// {
//   ComponentHierarcher cher;
//   explicit cher::hier<Position::Vel::Acc>;
//   explicit cher::hier<Position::Collision::Mask>
// ....
// }
```

User
```
// System
Component<ComponentExtractor<Position>> position;// Extracted Component
```

Dependency
- Components <- Hierarchy Constructor -> Systems

- Output
- A single type that reflects the ComponentHierarchy


Problems
- How to represent the dependency between diffrent domain Components, such as position and collision dependencies

Let's just give it a try.

First Purpose
- Contruct Simple Dependencies: Acc->Vel->Pos
- Extractor : 
- Use :


[Main points]Problems
- The processing order between different domains (concepts) is not always directly derived from dependencies.

e.g.
- Dependencies within the same domain (concept): 
     - The data is processed sequentially from downstream to upstream.
     - Pos::Vel::Acc
     - 0.Input -> 1. Acc -> 2. Vel -> 3.Pos

- Dependencies between different domains (concepts)
     - Pos::Coll::Mask
     - 0. Pos -> 1. Mask -> 2. Coll

[Consideration]
- All data is assumed to belong to a unique primary domain.
     - e.g. PositionDomain (Pos, Vel, Acc) / CollisionDomain (Collision, Collision Mask)
- Dependencies between different domains are established by using a single data point in either domain as a branching point (connection point).
     - e.g. Collision domains are connected to the Pos data of the PositionDomain.

Therefore, a domain that has another domain upstream needs to be processed only after the processing of the upstream domain has been completed.

[Solution]
- Domain dependencies explicitly define the dependency structure between different domains
- The processing order between different domains is determined by **the dependencies (hierarchical structure?) between those domains.**
- The processing order within a domain is determined by the component dependencies (hierarchical structure?)


## minimum implementation

---
API

- [ Component ]  
     - Use as usual in user-space logic.
```
struct Pos{};
struct Vel{};
struct Acc{};
```

TSCHEDULER API:  

- [ Descriptor]
     - dependency
     - ordering
     - phase
     - access pattern

```
// Wraps the regular type
using PosNode = Node<Pos>;
using DisNode = Node<Dis>;// distance
using VelNode = Node<Vel>;
using AccNode = Node<Acc>;

// link nodes and arcs
using Ontology_Vel_Depend = Arc<VelNode, AccNode>;
using Ontology_Pos_Depend = Arc<PosNode, DisNode, Ontology_Vel_Depend>;  // Graph Root

// construct graph
struct PhysTag{};
using Ontology_PysGraph = Graph<PhysTag, Ontology_Pos_Depend>;
```

- [ Compile-time Resolver ]
  - DAG resolve
  - topo sort
  - validation
  - cycle detection

```

```

---

### 1.
[A structure that seems necessary]:
- A system for building and outputting component dependencies expressed solely through templates.


[Minimum implementation]: 
- Building a Meta Hierarchy Structure for Data Structures Using Templates
- The process of iterating through that hierarchical structure in reverse order


[Useful technologies]:
- Compiler and linker dependency resolution (Topological sorting)
- consider how to extend component dependencies into a hierarchical structure and how to implement it.

### 1.5 
`Base on Implementation 1`

[Design Problems]  
- How to control the resolution order of another leaf with the same root
- How to handle duplicate nodes
- Should we use the order of ElementTs within the Graph as is?
- In LeafFirst, do you also reverse the order of the Graph elements themselves?

- **The current `ResolverDirection::RootFirst/LeafFirst` describes a method of interpreting a graph, not a search algorithm..**  
For example, interpreting a directed graph constructed as an ontology graph in reverse (i.e., as a graph with multiple roots, each with one child) can be interpreted as a task dependency.

[Concrete Problems]
- In the case of RootFirst, the order is Root->Leaf (leftmost)-> "Leftmost" refers to the order in which the template parameters are written.

The question to consider is:  
- Is it better to look at the children of the children first, then scan the younger siblings? (DPS)

Is it better to look at all the children first, then scan the children (grandchildren) of the children? (BFS)

[Consideration]

### 2. 
[Minimum implementation]:
- A system for building and parsing hierarchical relationships between domains.
- Connecting to the Component hierarchy


## Useful technorlogies 

- compile-time DAG 