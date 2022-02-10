//
// Created by Marcin Jarczewski on 08.02.2022.
//
#include "./SDL2-2.0.10/include/SDL.h"

#ifndef SOKOBAN_PLAYER_H
#define SOKOBAN_PLAYER_H

typedef struct player {
    int x, y;
    SDL_Rect box;
} player_t;

#endif //SOKOBAN_PLAYER_H
