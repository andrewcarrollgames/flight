# Flight

A multi-platform game engine focused on runtime performance and developer iteration speed.

## Philosophy

Flight is built around these core principles:

1. **Fast Iteration**: Hot-reload support for game code lets you see changes instantly without restarting
2. **Runtime Performance**: Zero-cost abstractions in release builds - plugins compile away to direct function calls
3. **Platform Agnostic**: Write once, run on Windows, Linux, macOS, and Web (via Emscripten)
4. **Extensibility**: Build your engine from composable extensions - ship only what you need

## Architecture

Flight uses a layered architecture that separates platform, engine, and application concerns:

```
┌─────────────────────────────────────────────┐
│  Platform (SDL3)                            │
│  - Window/Input/Audio                       │
│  - Rendering Backend                        │
│  - Memory Management                        │
└──────────────┬──────────────────────────────┘
               │
┌──────────────▼──────────────────────────────┐
│  Engine + Extensions                        │
│  - Plugin Manager (Hot Reload)              │
│  - Core Extensions (Physics, ECS, etc.)     │
│  - Tool Extensions (Editor, Profiler, etc.) │
└──────────────┬──────────────────────────────┘
               │
┌──────────────▼──────────────────────────────┐
│  Game (Extension or Plugin)                 │
│  - Your game logic                          │
│  - Static extension in release              │
│  - Hot-reloadable plugin in development     │
└─────────────────────────────────────────────┘
```

### Key Architectural Decisions

**Extensions vs Plugins**: Flight distinguishes between two types of code modules:

- **Extensions**: Statically linked components that define your engine's capabilities. These are the LEGO bricks of your development environment and shipping runtime. Extensions always ship with your game - if you include it, you need it. Examples: physics system, ECS, audio mixer, custom rendering features.

- **Plugins**: Development-only modules loaded dynamically for hot-reload iteration. These are purely development conceits - tools that need rapid iteration without full restarts. Examples: level editor, profiler, debug visualizer. Plugins never ship in release builds.

Your game can be either! In development, it's a hot-reloadable plugin. In release, it becomes a static extension.

**Macro Magic**: The `plugin_macros.h` system provides a unified API that automatically switches between hot-reloadable (indirect) and static (direct) calls based on build configuration:

```c
// In your code, just write:
PLATFORM_LOG("Hello, world!");

// Development build: calls through function pointer
// Release build: direct call to Platform_Log()
```

In the future, this will be generated automatically by the build system - extensions will describe their APIs, and macros will be generated for both hot-reload and static paths.

**Memory Model**: Flight uses a fully arena-based memory system with no `malloc`/`free`:

```
Root Arena (Virtual, 4GB reserved)
├─ Engine Frame Arena (Stack, 16MB) - resets each frame
├─ Entity Pool (Block, 10k entities)
└─ Game Arena (Bump, 256MB)
   ├─ Game Frame Arena (Stack, 4MB) - resets each frame
   ├─ Particle Pool (Block, 50k particles)
   └─ Audio Arena (Bump, 32MB)
```

All memory chains from a single OS-backed Virtual arena created at startup. Benefits:
- No fragmentation - arenas allocate linearly
- Cache friendly - related data packed together
- Easy debugging - visualize the entire memory tree
- Hot-reload safe - arena pointers persist across reloads
- Fast - bump allocation is just pointer increment
- Clear ownership - parent arenas own their children

**Performance**: With arenas, Flight achieves 15,000+ FPS in debug builds with zero game logic - plenty of headroom for actual gameplay!

## Building

Flight uses CMake with presets for different platforms and configurations.

### Prerequisites

- CMake 3.5 or newer
- C compiler (MSVC on Windows, Clang on macOS/Linux)
- SDL3 (included as submodule)
- Ninja build system
- Emscripten SDK (optional, for web builds)

### Quick Start

```bash
# Clone with submodules
git clone --recursive https://github.com/yourusername/flight.git
cd flight

# Configure and build (pick your platform)
cmake --preset windows-debug        # Windows development build
cmake --preset linux-debug          # Linux development build
cmake --preset macos-debug          # macOS development build

cmake --build --preset build-windows-debug
```

### Available Presets

Each platform has multiple configurations:

- **debug**: Full debug info, no optimizations, static game link
- **debug-hotreload**: Debug with hot-reload enabled (game is a DLL/SO)
- **release**: Fully optimized, static game link
- **relwithdebinfo**: Optimized with debug symbols (recommended for development)
- **minsizerel**: Optimized for size

Example presets:
- `windows-debug-hotreload` - Development with instant code reload
- `linux-relwithdebinfo` - Optimized but debuggable
- `emscripten-release` - Web deployment build

### Hot Reload Workflow

```bash
# Terminal 1: Build and run with hot reload
cmake --preset windows-debug-hotreload
cmake --build --preset build-windows-debug-hotreload
./build/debug-hotreload/bin/flight_debug_hr.exe

# Terminal 2: Make changes to game code
# Edit game/src/game.c
cmake --build --preset build-windows-debug-hotreload --target game

# Game automatically reloads in Terminal 1!
```

## Project Structure

```
flight/
├── platform/          # Platform abstraction layer (SDL3 backend)
│   ├── src/          
│   │   ├── sdl/              # SDL3 implementation
│   │   ├── platform_plugin_*.c  # Plugin loading (OS-specific)
│   │   ├── platform_memory_*.c  # Virtual memory (OS-specific)
│   │   └── arena.c           # Arena allocator implementations
│   └── include/
├── engine/           # Core engine systems
│   ├── src/
│   │   ├── engine.c          # Main engine loop
│   │   ├── plugin_manager.c  # Hot reload system
│   │   └── static_manifest.c # Extension registration
│   └── include/
├── extensions/       # Engine extensions (ship with game)
│   └── test/
│       ├── test_extension.c  # Example extension
│       └── test_extension_api.h
├── game/             # Your game code (extension or plugin)
│   ├── src/
│   │   └── game.c            # Game implementation
│   └── include/
├── shared/           # Interface contracts between layers
│   └── include/
│       ├── platform_api.h    # Platform services
│       ├── engine_api.h      # Engine services
│       ├── extension.h       # Extension interface
│       ├── plugin_api.h      # Plugin interface
│       └── plugin_macros.h   # Hot-reload macros
└── CMakeLists.txt
```

## Writing Extensions

Extensions are the building blocks of your engine. They're statically compiled, always ship, and provide core functionality.

### Extension Interface

```c
#include "extension.h"
#include "platform_api.h"

// Your extension's API (what other code calls)
typedef struct MyExtensionAPI {
    void (*DoSomething)(int param);
    int (*GetValue)(void);
} MyExtensionAPI;

// Private state
static PlatformAPI* g_platform = NULL;
static int g_my_value = 0;

// API implementation
static void MyExt_DoSomething(int param) {
    g_my_value = param;
    g_platform->Log("Did something: %d", param);
}

static int MyExt_GetValue(void) {
    return g_my_value;
}

static MyExtensionAPI g_api = {
    .DoSomething = MyExt_DoSomething,
    .GetValue = MyExt_GetValue
};

// Extension lifecycle
static bool MyExt_Init(EngineAPI* engine, PlatformAPI* platform) {
    g_platform = platform;
    platform->Log("MyExtension initialized");
    return true;
}

static void MyExt_Update(float dt) {
    // Update logic (if needed)
}

static void MyExt_Shutdown(void) {
    g_platform->Log("MyExtension shutdown");
}

static void* MyExt_GetSpecificAPI(void) {
    return &g_api;
}

// Extension declaration - registered at startup
ExtensionInterface g_extension_myext = {
    .name = "MyExtension",
    .Init = MyExt_Init,
    .Update = MyExt_Update,
    .Shutdown = MyExt_Shutdown,
    .GetSpecificAPI = MyExt_GetSpecificAPI
};
```

### Using Extensions from Game/Plugins

```c
// In your game code
#include "plugin_macros.h"

// Access extension API (will be generated in future)
// For now, manually add to plugin_macros.h:
#ifdef ENABLE_HOT_RELOAD
    #define MYEXT_DO_SOMETHING(param) \
        do { \
            MyExtensionAPI* api = (MyExtensionAPI*)__engine_api()->GetExtensionAPI("MyExtension"); \
            if (api) api->DoSomething(param); \
        } while(0)
#else
    #define MYEXT_DO_SOMETHING(param) MyExt_DoSomething(param)
#endif

// Use it
void Game_Update(void* state, float dt) {
    MYEXT_DO_SOMETHING(42);
}
```

### Extension Guidelines

- **Always ship**: Extensions are non-negotiable - if you include it, you need it
- **Stateless when possible**: Store state in arenas, not globals
- **Version your APIs**: Breaking changes need version bumps (once we hit 1.0)
- **Update only if needed**: If your extension doesn't need per-frame updates, set `Update = NULL`

## Writing Game Code

Your game implements either the `ExtensionInterface` (for static builds) or `PluginAPI` (for hot-reload). All memory comes from arenas - no malloc!

```c
#include "game.h"
#include "game_context.h"
#include "plugin_macros.h"

// Initialize: allocate and set up your state from arenas
bool Game_Initialize(void** state, PlatformAPI* platform, EngineAPI* engine) {
    // Get root arena from platform
    Arena* root = PLATFORM_GET_ROOT_ARENA();
    
    // Create game's persistent arena
    Arena* game_arena = ARENA_CREATE_BUMP(root, MEGABYTES(256), DEFAULT_ALIGNMENT);
    ARENA_SET_DEBUG_NAME(game_arena, "Game");
    
    // Allocate game state from game arena
    GameState* gs = Arena_AllocType(game_arena, GameState);
    *state = gs;
    gs->arena = game_arena;
    
    // Create subsystem arenas
    gs->frame_arena = ARENA_CREATE_STACK(game_arena, MEGABYTES(4), DEFAULT_ALIGNMENT);
    ARENA_SET_DEBUG_NAME(gs->frame_arena, "Game::Frame");
    
    // Use platform services through macros
    gs->window = PLATFORM_CREATE_WINDOW("My Game", 1280, 720, PLATFORM_RENDERER_OPENGL);
    gs->renderer = PLATFORM_CREATE_RENDERER(gs->window);
    
    return true;
}

// Update: called every frame with delta time
void Game_Update(void* state, float dt) {
    GameState* gs = (GameState*)state;
    
    // Reset frame arena at start of each frame
    ARENA_RESET(gs->frame_arena);
    
    // Allocate temp data - automatically freed next frame
    TempData* temp = Arena_AllocType(gs->frame_arena, TempData);
    
    // Your game logic here
    PLATFORM_LOG("FPS: %.2f", 1.0f / dt);
}

// Render: draw your game
void Game_Render(void* state) {
    GameState* gs = (GameState*)state;
    
    PLATFORM_RENDERER_CLEAR(gs->renderer);
    // ... draw stuff ...
    PLATFORM_RENDERER_PRESENT(gs->renderer);
}

// Shutdown: clean up your state
void Game_Shutdown(void** state) {
    GameState* gs = (GameState*)*state;
    
    PLATFORM_DESTROY_RENDERER(gs->renderer);
    PLATFORM_DESTROY_WINDOW(gs->window);
    
    // Destroy game arena (frees ALL game memory including frame_arena)
    ARENA_DESTROY(gs->arena);
    *state = NULL;
}
```

### Hot Reload Considerations

Your `GameState` persists across hot reloads, but your code doesn't:

✅ **Safe**: Plain old data in your state struct  
✅ **Safe**: Pointers to platform resources (windows, renderers)  
✅ **Safe**: Pointers to arena-allocated memory  
⚠️ **Careful**: Function pointers (will be invalid after reload)  
❌ **Unsafe**: Pointers to code or static data in your DLL

## Memory Management

Flight uses a **fully arena-based** memory system where `malloc`/`free` are discouraged:

### The One Root Arena
Platform creates a single Virtual arena (4GB reserved, commits pages on demand) at startup. All memory chains from this root.

### Arena Types

- **Virtual**: OS-backed root arena (platform-owned, commits memory as needed)
- **Bump**: Linear allocator for long-lived data (reset frees everything)
- **Stack**: Like Bump but with save/restore markers for scoped temps
- **Block**: Fixed-size pools for entities, particles, etc. *(coming soon)*
- **Multi-pool**: Power-of-2 size classes for general allocation *(coming soon)*
- **Scratch**: Thread-local temps for hot paths *(coming soon)*

### Usage Patterns

```c
// Get root from platform
Arena* root = PLATFORM_GET_ROOT_ARENA();

// Create persistent subsystem arena
Arena* physics_arena = ARENA_CREATE_BUMP(root, MEGABYTES(64), DEFAULT_ALIGNMENT);
ARENA_SET_DEBUG_NAME(physics_arena, "Physics");

// Create frame-temp arena (reset every frame)
Arena* frame_arena = ARENA_CREATE_STACK(physics_arena, MEGABYTES(4), DEFAULT_ALIGNMENT);

// Allocate objects
RigidBody* body = Arena_AllocType(physics_arena, RigidBody);

// Temporary allocations (scoped)
ARENA_TEMP(frame_arena) {
    float* temp_buffer = Arena_AllocArray(frame_arena, float, 1000);
    // Use temp_buffer
} // Automatically freed here

// Manual temp control
ArenaMarker mark = ARENA_MARK(frame_arena);
void* temp_data = ARENA_ALLOC(frame_arena, 512);
// ... use temp_data ...
ARENA_POP_TO(frame_arena, mark);  // Free back to marker

// Reset entire arena
ARENA_RESET(frame_arena);

// Destroy arena and all children
ARENA_DESTROY(physics_arena);
```

### Benefits

- **No fragmentation**: Linear allocation means perfect locality
- **Cache friendly**: Related data packed together in memory
- **Easy debugging**: Visualize entire memory tree from root
- **Hot-reload safe**: Arena pointers persist across reloads
- **Blazing fast**: Bump allocation is just pointer increment
- **Clear ownership**: Parent arenas own their children

## Roadmap

### Immediate (In Progress)
- [x] Hot reload system
- [x] Multi-platform support (Windows, Linux, macOS, Web)
- [x] Basic window/renderer abstraction
- [x] Arena-based memory system (Virtual + Bump + Stack)
- [x] Extension system foundation
- [ ] Auto-generated extension registration
- [ ] Auto-generated plugin macros
- [ ] Block arena (object pools)
- [ ] Input system

### Near Term
- [ ] Multi-pool arena (small block allocator)
- [ ] Scratch arenas (thread-local temps)
- [ ] Free-list + coalescing for Virtual arena
- [ ] Extension dependency system
- [ ] Extension API versioning
- [ ] Basic 2D rendering utilities
- [ ] Asset loading system

### Future
- [ ] Entity Component System (ECS)
- [ ] Job system for multithreading
- [ ] Advanced renderer abstraction (Vulkan/Metal/DX12)
- [ ] Audio system
- [ ] Physics integration
- [ ] Networking

## Design Decisions & FAQ

### Why extensions AND plugins?

**Extensions** are what your engine *is* - they define capabilities and always ship. **Plugins** are development tools - they load dynamically for hot-reload during iteration but never ship. Think of extensions as permanent features, plugins as temporary conveniences.

### Why not use an existing engine?

Flight is optimized for:
- **Programmers first**: Code-centric workflow, minimal "engine magic"
- **Full control**: No black boxes, understand every system
- **Fast iteration**: Hot reload without engine restart
- **Minimal overhead**: Extensions compile to direct calls in release builds

### What about scripting?

Not planned initially. C gives you the performance and control needed for game engines. If scripting is needed later, it can be added as an extension.

### How do I add a new extension?

1. Create `extensions/myext/myext_extension.c`
2. Implement `ExtensionInterface` 
3. Add to `engine/src/static_manifest.c` (will be automatic soon)
4. Create API header and add macros to `plugin_macros.h` (will be generated soon)
5. Rebuild

### Can I make commercial games with Flight?

Yes! MIT license - do whatever you want. Attribution appreciated but not required.

## License

MIT License - see [LICENSE](LICENSE) for details

## Contributing

Flight is in early development. Contributions welcome, but expect rapid changes to core APIs until we hit 1.0.

---

*"Fast code, fast iteration, no compromises."*
