//
// Created by Marcin Jarczewski on 08.02.2022.
//

extern "C" {
#include"./SDL2-2.0.10/include/SDL.h"
#include"./SDL2-2.0.10/include/SDL_main.h"
}

#ifndef SOKOBAN_GRAPHICS_H
#define SOKOBAN_GRAPHICS_H

typedef struct field {
    SDL_Surface *empty, *chest, *chestDest, *wall, *chestAtDest;
} field_t;

typedef struct playerSprites {
    SDL_Surface *p;

    SDL_Surface *upMove[3];
    SDL_Surface *downMove[3];
    SDL_Surface *leftMove[3];
    SDL_Surface *rightMove[3];

    SDL_Surface *sprites[4][3]; // TODO: move all sprites to one array
} pSprites_t ;

typedef struct graphics {
    SDL_Surface *charset, *screen;
    SDL_Texture *scrtex;
    SDL_Renderer *renderer;
    SDL_Window *window;

    field_t field;
    pSprites_t pSprites;
} graphics_t;

const int PLAYER_WIDTH = 64;
const int PLAYER_HEIGHT = 64;

#endif //SOKOBAN_GRAPHICS_H
