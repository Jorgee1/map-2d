#include <stdio.h>
#include <stdbool.h>

#include <SDL.h>

#include "utils.h"
#include "sprite.h"

int main(int argc, char *argv[])
{
    Colors colors;
    colors.red   = (SDL_Color) {0xFF, 0x00, 0x00, 0xFF};
    colors.green = (SDL_Color) {0x00, 0xFF, 0x00, 0xFF};
    colors.blue  = (SDL_Color) {0x00, 0x00, 0xFF, 0xFF};
    colors.white = (SDL_Color) {0xFF, 0xFF, 0xFF, 0xFF};
    colors.black = (SDL_Color) {0x00, 0x00, 0x00, 0xFF};

    int upscale = 2;
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

    int cell_size = 16;
    const int n = 2;

    char *tileset_path[n] = {
        "assets/textures/inside.png",
        "assets/textures/outside.png"
    };

    SpriteSheet spritesheet[n];
    TileSet tileset[n];

    for (int i = 0; i < n; i++)
    {
        SDL_Renderer *renderer = screen.renderer;
        SpriteSheet *temp_sheet = &spritesheet[i];
        TileSet *temp_tileset = &tileset[i];
        char *path = tileset_path[i];

        load_spritesheet(temp_sheet, renderer, path, cell_size);
        load_tileset(temp_tileset, temp_sheet);
    }


    int nw = 3;
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


        SDL_RenderCopy(
            screen.renderer,
            spritesheet[0].texture,
            NULL,
            &spritesheet[0].rect
        );

        int n_tiles = tileset[0].n;
        int carry_y = 0;
        for (int y = 0; y < n_tiles/2; y++)
        {
            int carry_x = 0;
            for (int x = 0; x < nw; x++)
            {
                int index = x + y * nw;
                if (index < n_tiles)
                {
                    Sprite *sprite = &tileset[0].sprites[x + y * nw];
                    SDL_Rect rect;
                    rect.x = x * sprite->rect.w * upscale + carry_x + 10;
                    rect.y = y * sprite->rect.h * upscale + carry_y;
                    rect.w = sprite->rect.w * upscale;
                    rect.h = sprite->rect.h * upscale;

                    rect.y += spritesheet[0].rect.h + 10;

                    SDL_RenderCopy(
                        screen.renderer,
                        sprite->texture,
                        &sprite->rect,
                        &rect
                    );
                    carry_x += 10;
                }
            }
            carry_y += 10;
        }
        
        SDL_RenderPresent(screen.renderer);
    }

    for (int i = 0; i < n; i++)
    {
        delete_spritesheet(&spritesheet[i]);
        delete_tileset(&tileset[i]);
    }

    SDL_DestroyRenderer(screen.renderer);
    SDL_DestroyWindow(screen.window);
    SDL_Quit();

    screen.renderer = NULL;
    screen.window = NULL;
    return 0;
}