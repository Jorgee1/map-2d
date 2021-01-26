#include <stdio.h>
#include <stdbool.h>

#include <SDL.h>

#include "utils.h"


int main(int argc, char *argv[])
{
    Colors colors;
    colors.red   = (SDL_Color) {0xFF, 0x00, 0x00, 0xFF};
    colors.green = (SDL_Color) {0x00, 0xFF, 0x00, 0xFF};
    colors.blue  = (SDL_Color) {0x00, 0x00, 0xFF, 0xFF};
    colors.white = (SDL_Color) {0xFF, 0xFF, 0xFF, 0xFF};
    colors.black = (SDL_Color) {0x00, 0x00, 0x00, 0xFF};

    Screen screen;
    int init = init_screen(
        &screen,
        "Map-2D",
        800, 600,
        colors.white,
        SDL_INIT_VIDEO,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if (init) return 1;

    while (screen.exit == SDL_GAME_RUN)
    {
        while (true)
        {
            SDL_Event event;
            if (SDL_PollEvent(&event) == 0) break;
            if (event.type == SDL_QUIT) screen.exit = SDL_GAME_EXIT;
        }
        set_render_draw_color(screen.renderer, colors.black);
        SDL_RenderClear(screen.renderer);
        
        SDL_RenderPresent(screen.renderer);
    }

    SDL_DestroyRenderer(screen.renderer);
    SDL_DestroyWindow(screen.window);
    SDL_Quit();

    screen.renderer = NULL;
    screen.window = NULL;
    return 0;
}