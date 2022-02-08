//
// Created by Marcin Jarczewski on 08.02.2022.
//

#include "player.h"
#include "colors.h"
#include "graphics.h"

#ifndef SOKOBAN_GAME_H
#define SOKOBAN_GAME_H

typedef struct variables {
    int t1, t2, quit, frames, reset;
    double delta, worldTime, fpsTimer, fps, distance, etiSpeed;
    graphics_t vfx;
    colors_t colors;
    player_t player;
} var_t;

int startProgram();

#endif //SOKOBAN_GAME_H
