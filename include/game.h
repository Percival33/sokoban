//
// Created by Marcin Jarczewski on 08.02.2022.
//
#include "board.h"
#include "consts.h"
#include "player.h"
#include "colors.h"
#include "graphics.h"
#include "chest.h"

#ifndef SOKOBAN_GAME_H
#define SOKOBAN_GAME_H

typedef struct result {
    int score;
    double time;
    char name[MAX_TEXT_LENGTH];
} result_t;

typedef struct variables {
    Uint32 t1, t2, quit, frames, reset, moves;
    double delta, worldTime, fpsTimer, fps;

    board_t board;

    char levelName[MAX_LEVEL_NAME_LENGTH];

    graphics_t vfx;
    colors_t colors;
    player_t player;

    chests_t chests;
    dests_t dests;
} var_t;

int startProgram();

#endif //SOKOBAN_GAME_H
