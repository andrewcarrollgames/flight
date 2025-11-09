#include "engine.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <platform.h>
#include <stdio.h>

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

  Engine_Update();
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
