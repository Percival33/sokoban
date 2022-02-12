//
// Created by Marcin Jarczewski on 08.02.2022.
//

#include "../include/game.h"

extern "C" {
#include"SDL.h"
#include"SDL_main.h"
}

// main
#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char **argv) {
   return startProgram();
};

