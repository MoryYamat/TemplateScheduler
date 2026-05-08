Input or available information for this library
- View <Component> 
     - selected 
     - required
     - excluded

Output


- Execution Order
e.g.
UpdateIntent();
     UpdateMovementIntent();
UpdateMovement();
     UpdateMovementAcceleration();
     UpdateMovementSpeed();
     UpdateEntityPosition();

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

[A structure that seems necessary]:
- A ComponentHierarchy construction and output system entirely represented by templates.
- 




[Minimum implementation]: 
- Building a Meta Hierarchy Structure for Data Structures Using Templates
- The process of iterating through that hierarchical structure in reverse order

[Useful technologies]:
- Compiler and linker dependency resolution (Topological sorting)
- 