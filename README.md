# Flight

A multi-platform game engine focused on runtime performance and developer iteration speed.

## Philosophy

Flight is built around three core rules:

1. **Protect the Heart**: Platform and engine core stay lean, clean C. No language soup, no build complexity, no clever magic.

2. **Developer Flexibility**: Hot-reload everything during development. Use any language for plugins. Add extensions easily. Configure via presets, not #ifdefs.

3. **Player Performance**: Release builds are static, direct calls, zero overhead. Small executables. No runtime indirection. Arena allocations = cache-friendly.

These rules reinforce each other: Extensions are written in C (protects the heart), hot-reload during development (flexibility), and compile to direct calls in release (performance).

## Architecture

```
┌───────────────────────────────────────────────┐
│  Platform (SDL3)                              │
│  - Window/Input/Audio                         │
│  - Memory Management (Arenas)                 │
│  - Logging                                    │
└──────────────────┬────────────────────────────┘
                   │
┌──────────────────▼────────────────────────────┐
│  Engine + Extensions (opt-in)                 │
│  - Plugin Manager (Hot Reload)                │
│  - Extensions: Input, 2D Renderer, ECS, etc.  │
└──────────────────┬────────────────────────────┘
                   │
┌──────────────────▼────────────────────────────┐
│  Game Module                                  │
│  - Your game logic                            │
│  - C (static or plugin) or any language (plugin) │
└───────────────────────────────────────────────┘
```

### The Core Is Modular

Flight's "engine" isn't a monolith - it's a collection of small, optional pieces:

- **Platform Layer**: Always present. Provides windowing, arena-based memory, logging, and OS abstraction.

- **Engine Core**: Minimal. Just the plugin manager for hot-reload support.

- **Extensions**: Optional C modules providing engine capabilities (input, rendering, ECS, physics wrappers). **You link only what you need.** Unused extensions aren't compiled - zero bytes, zero overhead.

- **Game Module**: Your application code. Can be statically linked C (fast recompile) or a hot-reloadable plugin in any language (instant iteration).

**Key insight:** Extensions ARE the optional parts of the engine. Your game module USES the platform and whatever extensions you've linked.

#### Zero-Cost Extensions

```
Your Game Executable
├─ Platform (always)
├─ Extensions (YOU choose)
│  ├─ input       ✓ linked
│  ├─ renderer_2d ✓ linked  
│  ├─ renderer_3d ✗ not linked = not compiled
│  └─ networking  ✗ not linked = not compiled
└─ Game Module
```

If you don't link an extension, it's not in your executable. Zero bytes, zero overhead.

### How Hot Reload Works

Generated macros switch behavior based on build mode:

```c
// Your game code:
INPUT_IS_KEY_PRESSED(KEY_SPACE);

// Development (hot-reload): indirect call via function pointer
// Release (static): direct call to Input_IsKeyPressed()
```

Write code once, works in both modes. Hot reload during development, zero-cost in release.

### Memory Model

Flight uses fully arena-based memory (no malloc/free):

```
Root Arena (Virtual, 4GB reserved)
├─ Engine Frame Arena (resets each frame)
└─ Game Arena
   ├─ Game Frame Arena (resets each frame)
   └─ Subsystem Arenas (entities, particles, etc.)
```

Benefits: no fragmentation, cache-friendly, hot-reload safe, fast allocation, clear ownership.

## Building

### Quick Start

```bash
git clone --recursive https://github.com/yourusername/flight.git
cd flight

# Configure and build
cmake --preset windows-debug
cmake --build --preset build-windows-debug
```

### Build Configuration

Two independent flags control build behavior:

**ENABLE_HOT_RELOAD** (workflow choice)
- `OFF` (default): Game statically linked, full recompile workflow
- `ON`: Game is DLL, instant hot-reload

**SHIP_PLUGIN_GAME** (release distribution, only when ENABLE_HOT_RELOAD=OFF)
- `OFF` (default): Game baked into .exe
- `ON`: Game ships as separate DLL (enables modding)

### Available Presets

- `windows-debug` / `linux-debug` / `macos-debug` - Static link, fast recompile
- `windows-debug-hotreload` - Hot reload enabled
- `windows-release` - Optimized, static link
- `emscripten-release` - Web builds

### Hot Reload Workflow

```bash
# Terminal 1: Run with hot reload
cmake --preset windows-debug-hotreload
cmake --build --preset build-windows-debug-hotreload
./build/debug-hotreload/bin/flight_debug_hr.exe

# Terminal 2: Edit game code, rebuild
cmake --build --preset build-windows-debug-hotreload --target game
# Changes appear instantly in Terminal 1
```

## Writing Extensions

Extensions are modular pieces of Flight's engine core. Write an extension when adding reusable engine capabilities (renderer, input, ECS, physics wrapper) that multiple games would use.

### Extension Interface

```c
#include "extension.h"

typedef struct MyExtensionAPI {
    void (*DoSomething)(int param);
} MyExtensionAPI;

static MyExtensionAPI g_api = {
    .DoSomething = MyExt_DoSomething
};

static bool MyExt_Init(EngineAPI* engine, PlatformAPI* platform) {
    platform->Log("MyExtension initialized");
    return true;
}

static void* MyExt_GetSpecificAPI(void) {
    return &g_api;
}

ExtensionInterface g_extension_myext = {
    .name = "MyExtension",
    .Init = MyExt_Init,
    .Update = NULL,  // Optional
    .Shutdown = MyExt_Shutdown,
    .GetSpecificAPI = MyExt_GetSpecificAPI
};
```

Extensions must be C, are statically compiled, and provide their API via `GetSpecificAPI()`. Generated macros make them callable from hot-reloadable game code.

## Writing Game Code

Your game implements the `PluginAPI` interface. Can be written in C (static or plugin) or any language with C FFI (must be plugin).

### C Game Example

```c
#include "game.h"
#include "plugin_macros.h"

bool Game_Initialize(void** state, PlatformAPI* platform, EngineAPI* engine) {
    // Get root arena and create game arena
    Arena* root = PLATFORM_GET_ROOT_ARENA();
    Arena* game_arena = ARENA_CREATE_BUMP(root, MEGABYTES(256), DEFAULT_ALIGNMENT);
    
    GameState* gs = ARENA_ALLOC(game_arena, sizeof(GameState));
    *state = gs;
    gs->arena = game_arena;
    
    // Create window, renderer, etc.
    gs->window = PLATFORM_CREATE_WINDOW("My Game", 1280, 720, PLATFORM_RENDERER_OPENGL);
    gs->renderer = PLATFORM_CREATE_RENDERER(gs->window);
    
    return true;
}

void Game_Update(void* state, float dt) {
    GameState* gs = (GameState*)state;
    // Game logic using extension macros:
    // INPUT_IS_KEY_PRESSED(KEY_SPACE);
}

void Game_Render(void* state) {
    GameState* gs = (GameState*)state;
    PLATFORM_RENDERER_CLEAR(gs->renderer);
    // Draw calls
    PLATFORM_RENDERER_PRESENT(gs->renderer);
}

void Game_Shutdown(void** state) {
    GameState* gs = (GameState*)*state;
    ARENA_DESTROY(gs->arena);  // Frees all game memory
    *state = NULL;
}
```

All memory comes from arenas. No malloc/free needed.

### Polyglot Games (Plugin Only)

Non-C games must be built as plugins (DLLs). Implement the `PluginAPI` interface in your language:

**Rust Example:**

```rust
// Cargo.toml: crate-type = ["cdylib"]

#[no_mangle]
pub extern "C" fn GetPluginAPI() -> *mut PluginAPI {
    static mut API: PluginAPI = PluginAPI {
        version: 1,
        name: b"Rust Game\0".as_ptr() as *const i8,
        init: Some(game_init),
        update: Some(game_update),
        render: Some(game_render),
        shutdown: Some(game_shutdown),
        // ...
    };
    unsafe { &mut API }
}
```

Supported languages: Rust, Zig, C++, Odin - anything with C FFI. Build as DLL, place in Flight's build directory, and the engine loads it automatically.

You can use Flight's arenas from any language via the Platform API, or use your language's native allocator.

## Memory Management

Flight uses arena-based memory exclusively:

**Arena Types:**
- **Virtual**: OS-backed root (commits pages on demand)
- **Bump**: Linear allocator, reset frees everything
- **Stack**: Push/pop with save/restore markers
- **Block/Multi-pool/Scratch**: Coming soon

**Usage:**

```c
Arena* root = PLATFORM_GET_ROOT_ARENA();
Arena* game_arena = ARENA_CREATE_BUMP(root, MEGABYTES(256), DEFAULT_ALIGNMENT);

// Allocate
void* data = ARENA_ALLOC(game_arena, 1024);

// Scoped temp allocations
ARENA_TEMP(frame_arena) {
    void* temp = ARENA_ALLOC(frame_arena, 512);
    // Auto-freed at scope end
}

// Reset or destroy
ARENA_RESET(frame_arena);
ARENA_DESTROY(game_arena);  // Frees arena and all children
```

Benefits: no fragmentation, cache-friendly, hot-reload safe, fast, clear ownership.

## Shipping Your Game

### Custom Presets

Create `MyGamePresets.json`:

```json
{
    "version": 6,
    "include": ["CMakePresets.json"],
    "configurePresets": [{
        "name": "my-game-release",
        "inherits": "windows-release",
        "cacheVariables": {
            "SHIP_PLUGIN_GAME": "OFF"  // Single .exe
        }
    }]
}
```

### Distribution Options

**Static (SHIP_PLUGIN_GAME=OFF):**
- Single executable
- Maximum performance
- Simpler deployment

**Plugin (SHIP_PLUGIN_GAME=ON):**
- Game ships as DLL alongside executable
- Enables modding/patching
- Multiple files to distribute

## Roadmap

### Current (In Progress)
- [x] Hot reload system
- [x] Multi-platform (Windows, Linux, macOS, Web)
- [x] Arena memory (Virtual + Bump + Stack)
- [ ] Auto-generated extension registration
- [ ] Auto-generated plugin macros
- [ ] Block arena
- [ ] Input system

### Near Term
- Multi-pool arena, scratch arenas
- Extension dependency/versioning
- 2D rendering utilities
- Asset loading

### Future
- ECS, job system
- Advanced renderers (Vulkan/Metal/DX12)
- Audio, physics, networking

## Key Questions

**When to make an extension?**
- Multiple games would use it
- It's a platform abstraction
- Want it statically compiled for performance

If game-specific or experimental, keep it in your game code.

**Can I use third-party libraries?**
Yes. Vendor them in your game (stb_image.h, Dear ImGui, Box2D, etc.). Extensions are for shared engine capabilities, not required for everything.

**How does hot-reload handle state?**
Arena-allocated memory persists across reloads. Avoid storing function pointers or static variables in your state struct.

**Performance impact of hot-reload?**
Development: ~1ns per API call (indirect through function pointer). Release: zero overhead (direct calls, compiler can inline).

**Can I still use malloc?**
Yes, but discouraged. Use arenas for game data, malloc only for interop with libraries that require it.

See [FAQ.md](FAQ.md) for more questions.

## License

MIT License - see [LICENSE](LICENSE) for details

## Contributing

Flight is in early development. Contributions welcome, but expect rapid changes until 1.0.

---

*"Fast code, fast iteration, no compromises."*
