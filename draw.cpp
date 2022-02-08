//
// Created by Marcin Jarczewski on 08.02.2022.
//

#include "draw.h"

// draw a text txt on surface screen, starting from the point (x, y)
// charset is a 128x128 bitmap containing character images
void drawString(SDL_Surface *screen, int x, int y, const char *text,
                SDL_Surface *charset) {
    int px, py, c;
    SDL_Rect s, d;
    s.w = 8;
    s.h = 8;
    d.w = 8;
    d.h = 8;
    while(*text) {
        c = *text & 255;
        px = (c % 16) * 8;
        py = (c / 16) * 8;
        s.x = px;
        s.y = py;
        d.x = x;
        d.y = y;
        SDL_BlitSurface(charset, &s, screen, &d);
        x += 8;
        text++;
    };
};


// draw a surface sprite on a surface screen in point (x, y) (top-left corner)
void drawSurface(SDL_Surface *screen, SDL_Surface *sprite, int x, int y) {
    SDL_Rect dest;
    dest.x = x;
    dest.y = y;
    dest.w = sprite->w;
    dest.h = sprite->h;
    SDL_BlitSurface(sprite, NULL, screen, &dest);
};


// draw a single pixel
void drawPixel(SDL_Surface *surface, int x, int y, Uint32 color) {
    int bpp = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
    *(Uint32 *)p = color;
};


// draw a vertical (when dx = 0, dy = 1) or horizontal (when dx = 1, dy = 0) line
void drawLine(SDL_Surface *screen, int x, int y, int l, int dx, int dy, Uint32 color) {
    for(int i = 0; i < l; i++) {
        drawPixel(screen, x, y, color);
        x += dx;
        y += dy;
    };
};


// draw a rectangle of size l by k
void drawRectangle(SDL_Surface *screen, int x, int y, int l, int k,
                   Uint32 outlineColor, Uint32 fillColor) {
    int i;
    drawLine(screen, x, y, k, 0, 1, outlineColor);
    drawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
    drawLine(screen, x, y, l, 1, 0, outlineColor);
    drawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
    for(i = y + 1; i < y + k - 1; i++)
        drawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
};