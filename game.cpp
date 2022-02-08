//
// Created by Marcin Jarczewski on 08.02.2022.
//

#include <stdio.h>
#include <math.h>

#include "game.h"
#include "draw.h"
#include "colors.h"
#include "consts.h"
#include "graphics.h"

extern "C" {
#include"./SDL2-2.0.10/include/SDL.h"
#include"./SDL2-2.0.10/include/SDL_main.h"
}

void terminateProgram(graphics_t *vfx) {
    SDL_FreeSurface(vfx->charset);
    SDL_FreeSurface(vfx->screen);
    SDL_DestroyTexture(vfx->scrtex);
    SDL_DestroyRenderer(vfx->renderer);
    SDL_DestroyWindow(vfx->window);
}

int loadBMP(graphics_t *vfx, char *path, SDL_Surface **surface) {
    *surface = SDL_LoadBMP(path);
    if(*surface == NULL) {
        printf("SDL_LoadBMP(%s) error: %s\n", path, SDL_GetError());
        terminateProgram(vfx);
        return ERROR;
    };

    return SUCCESS;
}


void display(var_t *game) {
    const double distance = game->distance;
    const double worldTime = game->worldTime;
    const double fps = game->fps;
    const int outlineColor = game->colors.RED;
    const int fillColor = game->colors.BLUE;
    const int backgroundColor = game->colors.BLACK;
    const graphics_t *vfx = &game->vfx;
    const player_t *player = &game->player;

    char text[128];

    SDL_FillRect(vfx->screen, NULL, backgroundColor);

    drawSurface(vfx->screen, vfx->eti, player->x, player->y);

    // info text
    drawRectangle(vfx->screen, 4, 4, SCREEN_WIDTH - 8, 36, outlineColor, fillColor);

    sprintf(text, "template for the second project, elapsed time = %.1lf s  %.0lf frames / s", worldTime, fps);
    drawString(vfx->screen, vfx->screen->w / 2 - strlen(text) * 8 / 2, 10, text, vfx->charset);

    sprintf(text, "Esc - exit, \030 - faster, \031 - slower");
    drawString(vfx->screen, vfx->screen->w / 2 - strlen(text) * 8 / 2, 26, text, vfx->charset);

    SDL_UpdateTexture(vfx->scrtex, NULL, vfx->screen->pixels, vfx->screen->pitch);
//		SDL_RenderClear(renderer);
    SDL_RenderCopy(vfx->renderer, vfx->scrtex, NULL, NULL);
    SDL_RenderPresent(vfx->renderer);
}

void setColors(graphics_t *vfx, colors_t *colors) {
    colors->BLACK = SDL_MapRGB(vfx->screen->format, 0x00, 0x00, 0x00);
//    colors->GREEN = SDL_MapRGB(vfx->screen->format, 0x00, 0xFF, 0x00);
    colors->RED = SDL_MapRGB(vfx->screen->format, 0xFF, 0x00, 0x00);
    colors->BLUE = SDL_MapRGB(vfx->screen->format, 0x11, 0x11, 0xCC);
}

int initProgram(graphics_t *vfx) {
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("SDL_Init error: %s\n", SDL_GetError());
        return ERROR;
    }

//	rc = SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP, &window, &renderer);
    int rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0,
                                         &vfx->window, &vfx->renderer);
    if(rc != 0) {
        SDL_Quit();
        printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
        return ERROR;
    };

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(vfx->renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_SetRenderDrawColor(vfx->renderer, 0, 0, 0, 255);

    SDL_SetWindowTitle(vfx->window, WINDOW_TITLE);

    vfx->screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
                                       0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

    vfx->scrtex = SDL_CreateTexture(vfx->renderer, SDL_PIXELFORMAT_ARGB8888,
                                    SDL_TEXTUREACCESS_STREAMING,
                                    SCREEN_WIDTH, SCREEN_HEIGHT);

    SDL_ShowCursor(SDL_DISABLE);

    char path1[] = "../assets/cs8x8.bmp";
    char path2[] = "../assets/eti.bmp";
    int err = loadBMP(vfx, path1, &vfx->charset);
    int err2 = loadBMP(vfx, path2, &vfx->eti);

    if(err || err2) {
        return ERROR;
    }

    SDL_SetColorKey(vfx->charset, true, 0x000000);
    return SUCCESS;
}

void handleEvents(var_t *game) {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_KEYDOWN:
                if(event.key.keysym.sym == SDLK_ESCAPE) game->quit = 1;
                else if(event.key.keysym.sym == SDLK_UP) game->player.y -= (int)game->distance;
                else if(event.key.keysym.sym == SDLK_DOWN) game->player.y += (int)game->distance;
                else if(event.key.keysym.sym == SDLK_n) {
                    game->reset = 1;
                    game->quit = 1;
                }
                break;
            case SDL_KEYUP:
                game->etiSpeed = 1.0;
                break;
            case SDL_QUIT:
                game->quit = 1;
                break;
        };
    };

}

void initGame(var_t *game) {
    game->t1 = SDL_GetTicks();

    game->frames = 0;
    game->fpsTimer = 0;
    game->fps = 0;
    game->quit = 0;
    game->worldTime = 0;
    game->distance = 0;
    game->etiSpeed = 1;
    game->reset = 0;

    game->player.x = SCREEN_WIDTH/3;
    game->player.y = SCREEN_HEIGHT/3;
}

int gameLoop(var_t *game) {

    initGame(game);

    while(!game->quit) {
        game->t2 = SDL_GetTicks();

        // here t2-t1 is the time in milliseconds since
        // the last screen was drawn
        // delta is the same time in seconds
        game->delta = (game->t2 - game->t1) * 0.001;
        game->t1 = game->t2;

        game->worldTime += game->delta;

//        game->distance = 0;
        game->distance += game->etiSpeed * game->delta;

        game->fpsTimer += game->delta;
        if(game->fpsTimer > 0.5) {
            game->fps = game->frames * 2;
            game->frames = 0;
            game->fpsTimer -= 0.5;
        };


        handleEvents(game);

        display(game);
        game->frames++;
    };

    if(game->reset)
        return RESET;

    return QUIT;
}

int startProgram() {
    var_t game;

    if(initProgram(&game.vfx)) {
        return ERROR;
    }

    setColors(&game.vfx, &game.colors);

    int flag = SUCCESS;

    while(flag != QUIT) {
        flag = gameLoop(&game);
    }

    terminateProgram(&game.vfx);
    SDL_Quit();

    return SUCCESS;
}