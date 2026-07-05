# TSR & Codex 実験結果(26.05/31) から

## 課題

**実行構造は正しいがAppStateが万能化してしまった**

```
Agentは実行構造を正しく作った  
しかし所有構造は崩れた  
つまり、 Execution Constraints だけでは、Agent を十分に誘導できない 可能性がある
```

```cpp
// Resources
struct WindowHandle
{
};
struct OpenGLContext
{
};
struct OpenGLFunctions
{
};
struct ShaderProgram
{
};
struct VertexBuffer
{
};
struct VertexArray
{
};
struct TriangleMesh
{
};
struct Framebuffer
{
};

// CONTEXT
struct AppState
{
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 color;
    };

    GLFWwindow* window = nullptr;
    unsigned int shader_program = 0;
    unsigned int vertex_buffer = 0;
    unsigned int vertex_array = 0;
    int width = 800;
    int height = 600;
    bool running = true;

    std::array<Vertex, 3> vertices{
        Vertex{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.1f, 0.1f}},
        Vertex{{0.5f, -0.5f, 0.0f}, {0.1f, 1.0f, 0.1f}},
        Vertex{{0.0f, 0.5f, 0.0f}, {0.1f, 0.3f, 1.0f}},
    };
};
```

問題は、

Resources は正しく用意されたが、それらが、どこに所属するかわからなかった．

だから Agent は すべてを  
`AppState`に入れた

もし所有関係が以下のように明示されていれば、
```
WindowContext owns:
- WindowHandle
- OpenGLContext

RendererContext owns:
- ShaderProgram
- VertexBuffer
- VertexArray

MeshContext owns:
- TriangleMesh

FrameContext owns:
- Framebuffer
```

Agentは自然に
```
struct WindowContext
{
    GLFWwindow* window;
};

struct RendererContext
{
    unsigned int shader_program;
    unsigned int vertex_buffer;
    unsigned int vertex_array;
};

struct MeshContext
{
    std::array<Vertex, 3> vertices;
};

struct FrameContext
{
    // framebuffer-related state
};

struct AppContext
{
    WindowContext window;
    RendererContext renderer;
    MeshContext mesh;
    FrameContext frame;
    bool running;
};
```

のように分けやすくなるかもしれない．そして、task 側 も 
```
InitWindow
  context: WindowContext
  writes: WindowHandle, OpenGLContext

CreateTriangleGeometry
  context: RendererContext + MeshContext
  reads: TriangleMesh
  writes: VertexArray, VertexBuffer

DrawTriangle
  context: RendererContext + FrameContext
  reads: VertexArray, ShaderProgram
  writes: Framebuffer
```

のように書ける

重要なことは、  
所有関係は単なるドキュメントではなく、Context 制約の根拠 になること

例えば、

```
RendererContext owns ShaderProgram
```

と書かれていれば、`CreateShaderProgram` が `AppContext.shader_program` に直接書くのはおかしい、と言える．

また、
```
MeshContext owns TriangleMesh
RendererContext owns VertexBuffer
```
なら、`CreateTriangleGeometry`が`MeshContext`から読み、`RendererContext`に書く という責務が見える

つまり、所有関係を明示すれば
- resource の場所
- task のアクセス境界
- context の肥大化チェック
- 設計意図から外れた実装の検出

につながる

TSR 的には 今の `Effects<T>`が
```
Task -> reads/writes Resource
```

を表すので、次に足すべきは

```
Context -> owns Resource
Task -> uses Context
```

イメージは以下
```
using Layout =
    ContextGraph<
        Owns<WindowContext, WindowHandle, OpenGLContext>,
        Owns<RendererContext, ShaderProgram, VertexArray, VertexBuffer>,
        Owns<MeshContext, TriangleMesh>,
        Owns<FrameContext, Framebuffer>
    >
```

検証として
```
Task が writes する Resource は、Task が使う Context の owns に含まれているか
Task が reads する Resource は、Task が使う Context または明示された dependency Context に存在するか
AppContext が過剰に Resource を直接 owns していないか
```
のようなチェックができるようになる

## 参考になりそうな分野
- Ownership Type Systems
- Region Based Memory
- Capability Models
- DDD(Bounded Context)
- Architecture Description Languages


## 本質
- 実際に所有しているという関係を表すのは難しい
- なぜなら非侵襲性がコアだし、コンパイル時計算だから
- だから、このTMP 記述性自体が、その構造を明示化することが重要
- それによって、Agent/人間 が 意図を共有可能であり、直接 Machine-Executable であることを保証できることが重要
- また、(たとえruntime実体との乖離が多少あっても)Type-Level で `Validation Layer`の導入が可能であることは重要
- reflection が 導入できればより強くなる可能性がある


## 懸念点
- 懸念点は Context と System の連携
- 規模になる場合、制約は正しく機能するか
- どの抽象レベルの制約が適切かということ

例えば、ECSのContext の場合、World という 比較的大規模なシステムがContext として機能する．  
これを正しく、このライブラリレベルでどのように表現・検証(もしくは制約)することができるのか

## TSR の役割 (あらためて)
1. Scaffold: Agentが構造化されたコードを書きやすくする足場
2. Validator: 構造仮説と実装・Effects・Ownershipの矛盾を検出する
3. Negotiator: 人間・Agent・Machineの間で、構造案を比較・調整する共通表現

構造について交渉するための中間表現

## ライブラリの暗黙的機能
最初は非侵襲性はruntimeでの最適化にかなわないのであまり価値がないかもしれないと思っていました  
今はとても重要なのではないかと思い始めました．  
それは、Agentの能力を過度に制限しないことが一つ．  
また、Agentにライブラリ使用の目的を明言できることも重要．  
つまりこの役割分担(非侵襲性)は  
人間が使うライブラリにとっては機能的な側面しか持たないが、  
Agentにとっては、より文脈として機能するということが考えれる

