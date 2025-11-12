// Copyright (c) 2025 Andrew Carroll Games, LLC
// All rights reserved.

#ifndef FLIGHT_ENGINE_H
#define FLIGHT_ENGINE_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

bool Engine_Initialize(void);
void Engine_Update(float deltaTime);
void Engine_Render();
void Engine_Shutdown(void);

#ifdef __cplusplus
}
#endif

#endif
