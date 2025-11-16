# Flight

A multi-platform game engine focused on runtime performance and developer iteration speed.

## Philosophy

Flight is built around these core principles:

1. **Fast Iteration**: Hot-reload support for game code lets you see changes instantly without restarting
2. **Runtime Performance**: Zero-cost abstractions in release builds - the plugin system compiles away to direct function calls
3. **Platform Agnostic**: Write once, run on Windows, Linux, macOS, and Web (via Emscripten)
4. **Extensibility**: Platform backends are easy to add (SDL3 is already included). New tools and features are a plugin away.

## Architecture

Flight uses a layered plugin architecture that separates concerns while maintaining flexibility:

```
┌─────────────────────────────────────────┐
│  Platform (SDL3)                        │
│  - Window/Input/Audio                   │
│  - Rendering Backend                    │
│  - Memory Management (coming soon)      │
└─────────────┬───────────────────────────┘
              │
┌─────────────▼───────────────────────────┐
│  Engine                                 │
│  - Plugin Manager                       │
│  - Hot Reload System                    │
│  - Core Systems (ECS, Assets - future)  │
└─────────────┬───────────────────────────┘
              │
┌─────────────▼───────────────────────────┐
│  Game (Plugin)                          │
│  - Your game logic                      │
│  - Can be static or hot-reloadable      │
└─────────────────────────────────────────┘
```

### Key Architectural Decisions

**Plugin System**: Game code is loaded as a plugin (DLL/SO) in development builds, enabling hot reload. In release builds, it's statically linked for maximum performance with zero overhead.

**Shared Interface**: The `shared/` directory defines clean contracts between layers via `PlatformAPI` and `EngineAPI` structs. These contain function pointers in development builds and resolve to direct calls in release.

**Macro Magic**: `plugin_macros.h` provides a unified API that automatically switches between hot-reloadable (indirect) and static (direct) calls based on build configuration:

```c
// In your code, just write:
PLATFORM_LOG("Hello, world!");

// Development build: calls through function pointer
// Release build: direct call to Platform_Log()
```

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
│   │   └── vector2.c         # Math utilities
│   └── include/
├── engine/           # Core engine systems
│   ├── src/
│   │   ├── engine.c          # Main engine loop
│   │   └── plugin_manager.c  # Hot reload system
│   └── include/
├── game/             # Your game code (can be plugin or static)
│   ├── src/
│   │   └── game.c            # Game implementation
│   └── include/
├── shared/           # Interface contracts between layers
│   └── include/
│       ├── platform_api.h    # Platform services
│       ├── engine_api.h      # Engine services
│       ├── plugin_api.h      # Plugin interface
│       └── plugin_macros.h   # Hot-reload macros
└── CMakeLists.txt
```

## Writing Game Code

Your game is a plugin that implements the `PluginAPI` interface:

```c
#include "game.h"
#include "game_context.h"
#include "plugin_macros.h"

// Initialize: allocate and set up your state
bool Game_Initialize(void** state, PlatformAPI* platform, EngineAPI* engine) {
    GameState* gs = malloc(sizeof(GameState));  // Soon: Arena_Alloc()
    *state = gs;
    
    // Use platform services through macros
    gs->window = PLATFORM_CREATE_WINDOW("My Game", 1280, 720, PLATFORM_RENDERER_OPENGL);
    gs->renderer = PLATFORM_CREATE_RENDERER(gs->window);
    
    return true;
}

// Update: called every frame with delta time
void Game_Update(void* state, float dt) {
    GameState* gs = (GameState*)state;
    
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
    
    free(gs);  // Soon: Arena_Destroy()
    *state = NULL;
}
```

### Hot Reload Considerations

Your `GameState` persists across hot reloads, but your code doesn't:

✅ **Safe**: Plain old data in your state struct  
✅ **Safe**: Pointers to platform resources (windows, renderers)  
⚠️ **Careful**: Function pointers (will be invalid after reload)  
❌ **Unsafe**: Pointers to code or static data in your DLL

## Roadmap

### Immediate (In Progress)
- [x] Hot reload system
- [x] Multi-platform support (Windows, Linux, macOS, Web)
- [x] Basic window/renderer abstraction
- [ ] **Arena-based memory system** (next up!)

### Near Term
- [ ] Custom memory allocators (bump, stack, pool, scratch)
- [ ] Input system
- [ ] Basic 2D rendering utilities
- [ ] Asset loading system
- [ ] Tool plugins (level editor, profiler, etc.)

### Future
- [ ] Entity Component System (ECS)
- [ ] Job system for multithreading
- [ ] Advanced renderer abstraction (Vulkan/Metal/DX12)
- [ ] Audio system
- [ ] Physics integration
- [ ] Networking
- [ ] Generalized static/dynamic plugin system (any can be static in release)

## Design Decisions & FAQ

### Why plugins for everything?

Tools like level editors and profilers become first-class citizens that share the same API as your game. In development, they load dynamically. In release, you ship without them.

### Why not use an existing engine?

Flight is optimized for:
- **Programmers first**: Code-centric workflow, minimal "engine magic"
- **Full control**: No black boxes, understand every system
- **Fast iteration**: Hot reload without engine restart
- **Embedded targets**: Designed to run on consoles and constrained platforms

### What about scripting?

Not planned initially. C gives you the performance and control needed for game engines. If scripting is needed later, it can be added as a plugin.

### Memory management strategy?

Moving to a **fully arena-based** memory system where `malloc`/`free` are discouraged:
- Global VM-backed arena at the root
- Typed child arenas (bump, stack, pool, scratch)
- Frame allocators for temporary data
- Thread-local arenas for job systems
- Long-lived resource arenas for assets

This eliminates fragmentation, improves cache coherency, and makes hot reload safer.

## License

MIT License - see [LICENSE](LICENSE) for details

## Contributing

Flight is in early development. Contributions welcome, but expect rapid changes to core APIs.

---

*"Fast code, fast iteration, no compromises."*
