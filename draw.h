//
// Created by Marcin Jarczewski on 08.02.2022.
//

#include "./SDL2-2.0.10/include/SDL.h"
#include "graphics.h"
#include "player.h"
#include "board.h"
#include "chest.h"

#ifndef SOKOBAN_DRAW_H
#define SOKOBAN_DRAW_H

void drawString(SDL_Surface *screen, int x, int y, const char *text,
                SDL_Surface *charset);

void drawSurface(SDL_Surface *screen, SDL_Surface *sprite, int x, int y);

void drawPixel(SDL_Surface *surface, int x, int y, Uint32 color);

void drawLine(SDL_Surface *screen, int x, int y, int l, int dx, int dy, Uint32 color);

void drawRectangle(SDL_Surface *screen, int x, int y, int l, int k,
                   Uint32 outlineColor, Uint32 fillColor);

void drawBoard(const graphics_t *vfx, const chests_t *chests, const dests_t *dests, const player_t *player, const board_t *board, int t1);
#endif //SOKOBAN_DRAW_H
