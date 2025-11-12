// Copyright (c) 2025 Andrew Carroll Games, LLC
// All rights reserved.

#include "engine.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <platform.h>
#include <stdio.h>

#ifdef SDL_MAIN_USE_CALLBACKS
const float nanoSecondsToSeconds = (1.0f / 1000000000.0f);
uint64_t prevFrameTimeNS = 0;
uint64_t currentTimeNS = 0;
float deltaTime = 0.0f;

// Called once at the start of the application
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  *appstate = NULL;

  // Initialize SDL subsystems
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS)) {
    Platform_LogError("Unable to initialize SDL: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  if (!Engine_Initialize()) {
    Platform_LogError("Engine initialization failed!");
    return SDL_APP_FAILURE;
  }

  Platform_Log("Initialization complete.");
  return SDL_APP_CONTINUE;
}

// Called for every event that occurs
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) { // NOLINT (Function signatures match requirements)
  (void)appstate;

  switch (event->type) {
    case SDL_EVENT_QUIT: {
      Platform_Log("Quit event received. Exiting.");
      return SDL_APP_SUCCESS; // Return success to exit
    }
    case SDL_EVENT_WINDOW_CLOSE_REQUESTED: {
      Platform_Log("Window close requested.");
      return SDL_APP_SUCCESS; // Return success to exit
    }
    case SDL_EVENT_KEY_DOWN: {
      if (event->key.key == SDLK_ESCAPE) {
        return SDL_APP_SUCCESS;
      }
      break;
    }
    default: {
      // Platform_LogError("Unknown event type: %d", event->type);
      break;
    }
  }
  return SDL_APP_CONTINUE; // Continue the loop
}

// Called repeatedly for the main loop's "tick"
SDL_AppResult SDL_AppIterate(void *appstate) { // NOLINT (Function signatures match requirements)
  (void)appstate;

  currentTimeNS = Platform_GetTicksNS();
  deltaTime = (float)(currentTimeNS - prevFrameTimeNS) * nanoSecondsToSeconds;
  prevFrameTimeNS = currentTimeNS;

  Engine_Update(deltaTime);
  Engine_Render();

  return SDL_APP_CONTINUE; // Continue the loop
}

// Called once when the application is exiting
void SDL_AppQuit(void *appstate, SDL_AppResult result) { // NOLINT (Function signatures match requirements)
  (void)appstate;
  (void)result; // The result of the application's execution

  Platform_Log("Application is quitting. Result: %d", result);
  Engine_Shutdown();
  SDL_Quit();
}

#else

int main(int argc, char* argv[]) {
  const float nanoSecondsToSeconds = (1.0f / 1000000000.0f);
  uint64_t prevFrameTimeNS = 0;
  uint64_t currentTimeNS = 0;
  float deltaTime = 0.0f;
  bool running = false;

  // Initialize SDL
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS)) {
    Platform_LogError("Failed to initialize SDL: %s", SDL_GetError());
    return 1;
  }

  // Initialize engine
  if (!Engine_Initialize()) {
    Platform_LogError("Failed to initialize engine");
    SDL_Quit();
    return 1;
  }

  Platform_Log("Initialization complete");

  // Main loop
  running = true;

  while (running) {
    currentTimeNS = Platform_GetTicksNS();
    deltaTime = (float)(currentTimeNS - prevFrameTimeNS) * nanoSecondsToSeconds;
    prevFrameTimeNS = currentTimeNS;

    // Poll events
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        running = false;
      }
      if (event.type == SDL_EVENT_KEY_DOWN) {
        if (event.key.key == SDLK_ESCAPE) {
          running = false;
        }
      }
      // Dispatch to engine/plugins if needed
      // engine_handle_event(&event);
    }

    // Update
    Engine_Update(deltaTime);

    // Render
    Engine_Render();

    // Optional: Frame rate limiting
    // platform_sleep(target_frame_time - delta_time);
  }

  // Cleanup
  Engine_Shutdown();
  SDL_Quit();

  Platform_Log("Application exiting");
  return 0;
}
#endif
