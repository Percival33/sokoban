//
// Created by Marcin Jarczewski on 12.02.2022.
//

#ifndef SOKOBAN_CHEST_H
#define SOKOBAN_CHEST_H

typedef struct chest {
    int row, col, onTarget;
}chest_t;

typedef struct point {
    int row, col;
} point_t;

typedef struct chests {
    int chestNum;
    chest_t *chests;
}chests_t;

typedef struct dests {
    int destNum;
    point_t *dests;
} dests_t;

#endif //SOKOBAN_CHEST_H
