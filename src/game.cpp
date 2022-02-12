//
// Created by Marcin Jarczewski on 08.02.2022.
//

#include <stdio.h>
#include <assert.h>
#include <math.h>

#include "../include/game.h"
#include "../include/draw.h"
#include "../include/colors.h"
#include "../include/consts.h"
#include "../include/graphics.h"

extern "C" {
#include"SDL.h"
#include"SDL_main.h"
}

void cleanUp(var_t *game) {
    free(game->chests.chests);
    free(game->dests.dests);
}

void freeSurface(SDL_Surface **surface) {
    if(*surface != NULL)
        SDL_FreeSurface(*surface);

    *surface = NULL;
}

void freeAssets(graphics_t *vfx) {

    for(int dir = LEFT; dir <= DOWN; dir++) {
        for(int frame = 0; frame < NUM_FRAMES; frame++) {
            if(vfx->pSprites.sprites[dir][frame] == nullptr)
                continue;

            freeSurface(&vfx->pSprites.sprites[dir][frame]);
        }
    }

    freeSurface(&vfx->field.empty);
    freeSurface(&vfx->field.chest);
    freeSurface(&vfx->field.chestDest);
    freeSurface(&vfx->field.wall);
    freeSurface(&vfx->field.chestAtDest);

    freeSurface(&vfx->winScreen);
}

void terminateProgram(var_t *game) {
    freeAssets(&game->vfx);
    cleanUp(game);

    SDL_FreeSurface(game->vfx.charset);
    SDL_FreeSurface(game->vfx.screen);
    SDL_DestroyTexture(game->vfx.scrtex);
    SDL_DestroyRenderer(game->vfx.renderer);
    SDL_DestroyWindow(game->vfx.window);
}

bool loadBMP(var_t *game, graphics_t *vfx, const char *path, SDL_Surface **surface) {
    *surface = SDL_LoadBMP(path);
    if(*surface == NULL) {
        printf("SDL_LoadBMP(%s) error: %s\n", path, SDL_GetError());
        terminateProgram(game);
        return true;
    };

    return false;
}

int loadAssets(var_t *game, graphics_t *vfx) {
    int err = 0;

    err |= loadBMP(game, vfx, "../assets/cs8x8.bmp", &vfx->charset);

    err |= loadBMP(game, vfx, "../assets/player/pDown1.bmp", &vfx->pSprites.sprites[DOWN][0]);
    err |= loadBMP(game, vfx, "../assets/player/pDown2.bmp", &vfx->pSprites.sprites[DOWN][1]);
    err |= loadBMP(game, vfx, "../assets/player/pDown3.bmp", &vfx->pSprites.sprites[DOWN][2]);

    err |= loadBMP(game, vfx, "../assets/player/pUp1.bmp", &vfx->pSprites.sprites[UP][0]);
    err |= loadBMP(game, vfx, "../assets/player/pUp2.bmp", &vfx->pSprites.sprites[UP][1]);
    err |= loadBMP(game, vfx, "../assets/player/pUp3.bmp", &vfx->pSprites.sprites[UP][2]);

    err |= loadBMP(game, vfx, "../assets/player/pLeft1.bmp", &vfx->pSprites.sprites[LEFT][0]);
    err |= loadBMP(game, vfx, "../assets/player/pLeft2.bmp", &vfx->pSprites.sprites[LEFT][1]);
    err |= loadBMP(game, vfx, "../assets/player/pLeft3.bmp", &vfx->pSprites.sprites[LEFT][2]);

    err |= loadBMP(game, vfx, "../assets/player/pRight1.bmp", &vfx->pSprites.sprites[RIGHT][0]);
    err |= loadBMP(game, vfx, "../assets/player/pRight2.bmp", &vfx->pSprites.sprites[RIGHT][1]);
    err |= loadBMP(game, vfx, "../assets/player/pRight3.bmp", &vfx->pSprites.sprites[RIGHT][2]);


    err |= loadBMP(game, vfx, "../assets/crate_01.bmp", &vfx->field.chest);
    err |= loadBMP(game, vfx, "../assets/wall.bmp", &vfx->field.wall);
    err |= loadBMP(game, vfx, "../assets/crate_27.bmp", &vfx->field.chestDest);
    err |= loadBMP(game, vfx, "../assets/empty.bmp", &vfx->field.empty);
    err |= loadBMP(game, vfx, "../assets/crate_12.bmp", &vfx->field.chestAtDest);

    err |= loadBMP(game, vfx, "../assets/winScreen.bmp", &vfx->winScreen);

    if(err) {
        return ERROR;
    }

    return SUCCESS;
}

void changePlayerSprite(const player_t *player, graphics_t *vfx, const int frame) {
    vfx->pSprites.p = vfx->pSprites.sprites[player->moveDir][frame];
}

void changeSprites(var_t *game) {

    if(game->player.hasMoved) {
        //change sprite to animate player
        float dt = (game->t1 - game->player.lastUpdate) * 0.001f;

        int framesToUpdate = floor(dt * ANIMATED_FPS);

        if (framesToUpdate > 0) {
            game->player.lastFrame += framesToUpdate;
            game->player.lastFrame %= NUM_FRAMES;
            game->player.lastUpdate = game->t1;
            game->player.hasMoved--;
        }

        changePlayerSprite(&game->player, &game->vfx, game->player.lastFrame);
    }
    else {
        changePlayerSprite(&game->player, &game->vfx, 0);
    }
    if (game->player.hasMoved < 0) {
        game->player.hasMoved = 0;
    }
}

void updateScreen(graphics_t *vfx) {
    SDL_UpdateTexture(vfx->scrtex, NULL, vfx->screen->pixels, vfx->screen->pitch);
//		SDL_RenderClear(vfx->renderer);
    SDL_RenderCopy(vfx->renderer, vfx->scrtex, NULL, NULL);
    SDL_RenderPresent(vfx->renderer);
}

void display(var_t *game) {
    const double worldTime = game->worldTime;
    const double fps = game->fps;
    const int backgroundColor = game->colors.BLACK;
    graphics_t *vfx = &game->vfx;
    board_t *board = &game->board;
    const int moves = game->moves;

    char levelName[MAX_LEVEL_NAME_LENGTH];
    strcpy(levelName, game->levelName);

    char text[MAX_TEXT_LENGTH];

    SDL_FillRect(vfx->screen, NULL, backgroundColor);

    changeSprites(game);

    drawBoard(vfx, &game->chests, &game->dests, &game->player, board, game->t1);

    sprintf(text, "%s, elapsed time = %.1lf s  %.0lf frames / s moves: %d", levelName, worldTime, fps, moves);
    drawString(vfx->screen, vfx->screen->w / 2 - strlen(text) * 8 / 2, 10, text, vfx->charset);

    updateScreen(vfx);

    game->vfx = *vfx;
}

void setColors(graphics_t *vfx, colors_t *colors) {
    colors->BLACK = SDL_MapRGB(vfx->screen->format, 0x00, 0x00, 0x00);
//    colors->GREEN = SDL_MapRGB(vfx->screen->format, 0x00, 0xFF, 0x00);
    colors->RED = SDL_MapRGB(vfx->screen->format, 0xFF, 0x00, 0x00);
    colors->BLUE = SDL_MapRGB(vfx->screen->format, 0x11, 0x11, 0xCC);
}

int initProgram(var_t *game, graphics_t *vfx) {
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

    if(loadAssets(game, vfx)) {
        return ERROR;
    }

    SDL_SetColorKey(vfx->charset, true, 0x000000);
    return SUCCESS;
}

int getChestId(var_t *game, int x, int y) {
    int row, col;

    for(int c = 0; c < game->chests.chestNum; c++) {
        row = game->chests.chests[c].row;
        col = game->chests.chests[c].col;

        if(row == y && col == x)
            return c;
    }

    return -1;
}

void movePlayer(var_t *game, int x, int y) {
    game->player.x = x;
    game->player.y = y;
    game->player.hasMoved = NUM_FRAMES;
    game->moves++;
}

void stayOnBoard(const int rows, const int cols, int *x, int *y) {
    if(*x < 0)
        *x = 0;

    if(*y < 0)
        *y = 0;

    if(*x >= cols)
        *x = cols - 1;

    if(*y >= rows)
        *y = rows - 1;
}

void checkChestOnTarget(var_t *game, int chestId) {
    int row, col, x, y;
    x = game->chests.chests[chestId].col;
    y = game->chests.chests[chestId].row;

    game->chests.chests[chestId].onTarget = 0;

    for(int d = 0; d < game->dests.destNum; d++) {
        row = game->dests.dests[d].row;
        col = game->dests.dests[d].col;
        if(row == y && col == x) {
            game->chests.chests[chestId].onTarget = 1;
        }
    }
}

void move(var_t *game, int dir) {
    int x, y, nextX, nextY, chestId;
    bool isWall, isNextWall, nextField;

    if(game->player.hasMoved)
        return;

    x = game->player.x + dx[dir];
    y = game->player.y + dy[dir];
    stayOnBoard(game->board.rows, game->board.cols, &x,&y);

    isWall = (game->board.grid[y][x] == WALL);
    chestId = getChestId(game,x,y);

    nextX = x + dx[dir];
    nextY = y + dy[dir];
    nextField = (getChestId(game, nextX, nextY) == -1);

    stayOnBoard(game->board.rows, game->board.cols, &nextX, &nextY);
    isNextWall = (game->board.grid[nextY][nextX] == WALL);

    nextField &= !isNextWall;

    if(!isWall) {
        if(chestId == -1){
            movePlayer(game, x, y);
        }
        else if(chestId != -1 && nextField) {
            game->chests.chests[chestId].row = nextY;
            game->chests.chests[chestId].col = nextX;
            checkChestOnTarget(game, chestId);
            movePlayer(game, x, y);
        }
    }

    game->player.moveDir = dir;
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
        case 'g':
            return CHEST_AT_DEST;
        default:
            return ERROR;
    }
}

void initChests(var_t *game) {
    int readChest, readDest;

    assert(game->dests.destNum == game->chests.chestNum);

    readChest = 0;
    readDest = 0;

    game->chests.chests = (chest_t*)malloc(game->chests.chestNum * sizeof(chest_t));
    game->dests.dests = (point_t*)malloc(game->dests.destNum * sizeof(point_t));

    for(int row = 0; row < game->board.rows; row++) {
        for(int col = 0; col < game->board.cols; col++) {
            switch(game->board.grid[row][col]) {
                case CHEST:
                    game->chests.chests[readChest].row = row;
                    game->chests.chests[readChest].col = col;
                    game->chests.chests[readChest].onTarget = 0;

                    readChest++;
                    break;
                case CHEST_AT_DEST:
                    game->chests.chests[readChest].row = row;
                    game->chests.chests[readChest].col = col;
                    game->chests.chests[readChest].onTarget = 1;

                    game->dests.dests[readDest].row = row;
                    game->dests.dests[readDest].col = col;

                    readChest++;
                    readDest++;
                    break;
                case CHEST_DEST:
                    game->dests.dests[readDest].row = row;
                    game->dests.dests[readDest].col = col;

                    readDest++;
                    break;
            }
        }
    }

    assert(readChest == readDest);
}

int loadLevel(var_t *game) {
    FILE *lvl;
    int tmp;
    char line[MAX_ROW_LENGTH];

    lvl = fopen("../levels/level2.txt", "r");

    if(lvl == NULL) {
        return ERROR;
    }
    strcpy(game->levelName, "Sokoban level 2");

    fgets(line, MAX_ROW_LENGTH, lvl);
    tmp = sscanf(line, "%d %d", &game->board.rows, &game->board.cols);
    assert(tmp == 2);

    game->board.grid = (int**)malloc(game->board.rows * sizeof(int*));

//TODO: fix readability
    for(int row = 0; row < game->board.rows; row++) {

        game->board.grid[row] = (int*)malloc(game->board.cols * sizeof(int));
        fgets(line, MAX_ROW_LENGTH, lvl);

        for(int col = 0; col < game->board.cols; col++) {
            tmp = getFieldType(line[col]);
            assert(tmp != ERROR);

            if(tmp == CHEST || tmp == CHEST_AT_DEST) {
                game->chests.chestNum++;
            }
            if(tmp == CHEST_DEST || tmp == CHEST_AT_DEST) {
                game->dests.destNum++;
            }

            game->board.grid[row][col] = tmp;
        }
    }


    initChests(game);

    //get position of a player
    fgets(line, MAX_ROW_LENGTH, lvl);
    tmp = sscanf(line, "%d %d", &game->player.x, &game->player.y);

    assert(tmp == 2);
    assert(game->board.grid[game->player.x][game->player.y] != CHEST);
    assert(game->board.grid[game->player.x][game->player.y] != WALL);
    assert(game->board.grid[game->player.x][game->player.y] != CHEST_AT_DEST);

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

    game->chests.chestNum = 0;
    game->dests.destNum = 0;

    game->player.x = 0;
    game->player.y = 0;

    game->player.hasMoved = 0;
    game->player.lastFrame = 0;
    game->player.lastUpdate = 0;
    game->player.moveDir = DOWN;
}

bool isWin(var_t *game) {
    bool win = true;
    for(int c = 0; c < game->chests.chestNum; c++) {
        win &= game->chests.chests[c].onTarget;
    }
    return win;
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

        if(isWin(game)) {
            int tmpX = (SCREEN_WIDTH - 250)/2;
            int tmpY = (SCREEN_HEIGHT - 250)/2;
            drawSurface(game->vfx.screen, game->vfx.winScreen, tmpX, tmpY);
            updateScreen(&game->vfx);

            SDL_Delay(3000);

            return QUIT;
        }

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

    if(initProgram(&game, &game.vfx)) {
        return ERROR;
    }

    setColors(&game.vfx, &game.colors);

    int flag = SUCCESS;

    while(flag != QUIT) {
        flag = gameLoop(&game);
    }

    terminateProgram(&game);
    SDL_Quit();

    return SUCCESS;
}