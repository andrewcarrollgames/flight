#ifndef GAME_H
#define GAME_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// forward declarations
typedef struct GameState GameState;

bool Game_Initialize(GameState *gameState);
void Game_Update(GameState *gameState);
void Game_Shutdown(GameState *gameState);

#ifdef __cplusplus
}
#endif

#endif
