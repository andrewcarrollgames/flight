#ifndef FLIGHT_ENGINE_H
#define FLIGHT_ENGINE_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

bool Engine_Initialize(void);
void Engine_Update(void);
void Engine_Render(void);
void Engine_Shutdown(void);

#ifdef __cplusplus
}
#endif

#endif
