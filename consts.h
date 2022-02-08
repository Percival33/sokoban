//
// Created by Marcin Jarczewski on 08.02.2022.
//

#ifndef SOKOBAN_CONSTS_H
#define SOKOBAN_CONSTS_H

enum STATUS {
    SUCCESS = 0,
    RESET,
    ERROR,
    QUIT
};

const char WINDOW_TITLE[] = "Sokoban";
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

#endif //SOKOBAN_CONSTS_H
