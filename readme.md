# TShceduler


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