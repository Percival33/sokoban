//
// Created by Marcin Jarczewski on 08.02.2022.
//

#include <stdio.h>
#include <assert.h>

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

int loadBMP(graphics_t *vfx, const char *path, SDL_Surface **surface) {
    *surface = SDL_LoadBMP(path);
    if(*surface == NULL) {
        printf("SDL_LoadBMP(%s) error: %s\n", path, SDL_GetError());
        terminateProgram(vfx);
        return 1;
    };

    return 0;
}

int loadAssets(graphics_t *vfx) {
    int err = 0;

    err |= loadBMP(vfx, "../assets/cs8x8.bmp", &vfx->charset);
    err |= loadBMP(vfx, "../assets/eti.bmp", &vfx->eti);

    err |= loadBMP(vfx, "../assets/player_1.bmp", &vfx->field.player);
    err |= loadBMP(vfx, "../assets/crate_01.bmp", &vfx->field.chest);
    err |= loadBMP(vfx, "../assets/wall.bmp", &vfx->field.wall);
    err |= loadBMP(vfx, "../assets/chestDest.bmp", &vfx->field.chestDest);
    err |= loadBMP(vfx, "../assets/empty.bmp", &vfx->field.empty);

    if(err) {
        return ERROR;
    }

    return SUCCESS;
}

void display(var_t *game) {
    const double worldTime = game->worldTime;
    const double fps = game->fps;
    const int backgroundColor = game->colors.BLACK;
    const graphics_t *vfx = &game->vfx;
    const player_t *player = &game->player;
    const int **board = (const int**)game->board;
    const int moves = game->moves;

    char levelName[MAX_LEVEL_NAME_LENGTH];
    strcpy(levelName, game->levelName);

    char text[MAX_TEXT_LENGTH];

    SDL_FillRect(vfx->screen, NULL, backgroundColor);

    drawBoard(vfx, player, board, game->rows, game->cols);

    // info text
//    drawRectangle(vfx->screen, 4, 4, SCREEN_WIDTH - 8, 36, outlineColor, fillColor);

    sprintf(text, "%s, elapsed time = %.1lf s  %.0lf frames / s moves: %d", levelName, worldTime, fps, moves);
    drawString(vfx->screen, vfx->screen->w / 2 - strlen(text) * 8 / 2, 10, text, vfx->charset);

//    sprintf(text, "Esc - exit, n - new game, arrows - movement");
//    drawString(vfx->screen, vfx->screen->w / 2 - strlen(text) * 8 / 2, 26, text, vfx->charset);

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

    if(loadAssets(vfx)) {
        return ERROR;
    }

    SDL_SetColorKey(vfx->charset, true, 0x000000);
    return SUCCESS;
}

void move(var_t *game, int dir) {
    int x = game->player.x;
    int y = game->player.y;

    x += dx[dir];
    y += dy[dir];

    if(x < 0)
        x = 0;

    if(y < 0)
        y = 0;

    if(x >= game->cols)
        x = game->cols - 1;

    if(y >= game->rows)
        y = game->rows - 1;

    int type = game->board[y][x];

    if(type != WALL && type != CHEST) {
        game->player.x = x;
        game->player.y = y;
        game->moves++;
    }

    game->player.box.x = game->player.x;
    game->player.box.y = game->player.y;
}

void handleEvents(var_t *game) {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_KEYDOWN:
                if(event.key.keysym.sym == SDLK_ESCAPE)
                    game->quit = 1;
                else if(event.key.keysym.sym == SDLK_n) {
                    game->reset = 1;
                    game->quit = 1;
                }
                else if(event.key.keysym.sym == SDLK_UP)
                    move(game, UP);
                else if(event.key.keysym.sym == SDLK_RIGHT)
                    move(game, RIGHT);
                else if(event.key.keysym.sym == SDLK_LEFT)
                    move(game, LEFT);
                else if(event.key.keysym.sym == SDLK_DOWN)
                    move(game, DOWN);
                break;
            case SDL_QUIT:
                game->quit = 1;
                break;
        };
    };

}

int getFieldType(char c) {
    switch(c) {
        case ' ':
            return EMPTY;
        case '#':
            return WALL;
        case 'c':
            return CHEST;
        case 'p':
            return PLAYER;
        case 'x':
            return CHEST_DEST;
        default:
            return ERROR;
    }
}

char printCell(int x) {
    switch(x) {
        case WALL:
            return '#';
        case PLAYER:
            return 'p';
        case EMPTY:
            return ' ';
        case CHEST:
            return 'c';
        case CHEST_DEST:
            return 'x';
        default:
            return '!';
    }
}

int loadLevel(var_t *game) {
    FILE *lvl;

    strcpy(game->levelName, "level");
    lvl = fopen("../levels/level.txt", "r");

    if(lvl == NULL) {
        return ERROR;
    }

    char line[MAX_ROW_LENGTH];
    fgets(line, MAX_ROW_LENGTH, lvl);

    assert( sscanf(line, "%d %d", &game->rows, &game->cols) == 2);

    game->board = (int**)malloc(game->rows * sizeof(int*));
    for(int row = 0; row < game->rows; row++) {

        game->board[row] = (int*)malloc(game->cols * sizeof(int));
        fgets(line, MAX_ROW_LENGTH, lvl);

        for(int col = 0; col < game->cols; col++) {
            game->board[row][col] = getFieldType(line[col]);

            if(line[col] == 'p') {
                game->player.x = col;
                game->player.y = row;
            }

            assert(game->board[row][col] != ERROR);
        }
    }

//    for(int r = 0; r < game->rows; r++) {
//        for(int c = 0; c < game->cols; c++) {
//            printf("%d\t",game->board[r][c]);
//        }
//        printf("\n");
//    }

    fclose(lvl);
    return 0;
}



void initGame(var_t *game) {
    game->t1 = SDL_GetTicks();

    game->frames = 0;
    game->fpsTimer = 0;
    game->fps = 0;
    game->quit = 0;
    game->worldTime = 0;
    game->reset = 0;
    game->moves = 0;

    game->player.x = 0;
    game->player.y = 0;

    game->player.box.w = PLAYER_WIDTH;
    game->player.box.h = PLAYER_HEIGHT;
}

int gameLoop(var_t *game) {

    initGame(game);

    if(loadLevel(game)) {
        return ERROR;
    }

    while(!game->quit) {
        game->t2 = SDL_GetTicks();

        // here t2-t1 is the time in milliseconds since
        // the last screen was drawn
        // delta is the same time in seconds
        game->delta = (game->t2 - game->t1) * 0.001;
        game->t1 = game->t2;

        game->worldTime += game->delta;

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