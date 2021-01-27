#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL.h>

#include "utils.h"
#include "sprite.h"



int main(int argc, char *argv[])
{

    if (argc != 3)
    {
        printf("Usage: map-2d path/to/texture size\n");
        return 1;
    }

    char* path = argv[1];
    int size = atoi(argv[2]);

    if (size <= 0) 
    {
        printf("size > 0\n");
        return 1;
    }

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

    int cell_size = 16;

    SpriteSheet spritesheet;
    int load = load_spritesheet(&spritesheet, screen.renderer, path, cell_size);
    if (load == 1) {
        printf("Unable to load %s\n", path);
        return 1;
    }

    TileSet tileset;
    load_tileset(&tileset, &spritesheet);


    // Tileset Layout
    int upscale = 3;

    int nw = 4;
    int n_tiles = tileset.n;

    SDL_Rect tile_rect[n_tiles];

    int carry_y = 0;
    int tile_selected = 0;

    for (int y = 0; y < n_tiles/2; y++)
    {
        int carry_x = 0;
        for (int x = 0; x < nw; x++)
        {
            int index = x + y * nw;
            if (index < n_tiles)
            {
                Sprite *sprite = &tileset.sprites[x + y * nw];

                tile_rect[index].x = x * sprite->rect.w * upscale + carry_x + 10;
                tile_rect[index].y = y * sprite->rect.h * upscale + carry_y;
                tile_rect[index].y += spritesheet.rect.h + 10;

                tile_rect[index].w = sprite->rect.w * upscale;
                tile_rect[index].h = sprite->rect.h * upscale;

                carry_x += 10;
            }
        }
        carry_y += 10;
    }

    Map map;
    init_map(&map, &tileset, size, size);
    map.size.x = cell_size * upscale * nw + 10 * nw + 10;


    SDL_Rect mouse = (SDL_Rect) {0, 0, 10, 10};
    bool mouse_b = false;

    while (screen.exit == SDL_GAME_RUN)
    {
        while (true)
        {
            SDL_Event event;
            if (SDL_PollEvent(&event) == 0) break;

            if (event.type == SDL_QUIT)
            {
                screen.exit = SDL_GAME_EXIT;
            }
            else if (event.type == SDL_MOUSEMOTION)
            {
                SDL_GetMouseState(&mouse.x, &mouse.y);
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    mouse_b = true;
                }
            }
            else if (event.type == SDL_MOUSEBUTTONUP)
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    mouse_b = false;
                }
            }
        }

        // Colition Detection
        for (int y = 0; y < map.size.h; y++)
        {
            for (int x = 0; x < map.size.w; x++)
            {
                SDL_Rect rect;
                rect.x = map.size.x + x * cell_size * upscale;
                rect.y = map.size.y + y * cell_size * upscale;
                rect.w = map.tiles->sprites->rect.w * upscale;
                rect.h = map.tiles->sprites->rect.w * upscale;
                if (check_collition(mouse, rect) == 1)
                {
                    if (mouse_b == true)
                    {
                        map.indexes[y][x] = tile_selected;
                    }
                    break;
                    break;
                }
            }
        }

        // Render
        set_render_draw_color(screen.renderer, colors.black);
        SDL_RenderClear(screen.renderer);

        map_render(screen.renderer, &map, upscale);

        SDL_RenderCopy(
            screen.renderer,
            spritesheet.texture,
            NULL,
            &spritesheet.rect
        );

        for (int i = 0; i < n_tiles; i++)
        {
            Sprite *sprite = &tileset.sprites[i];

            if (check_collition(mouse, tile_rect[i]) == 1)
            {
                if (mouse_b == true)
                {
                    tile_selected = i;
                }
                SDL_Rect bound;
                bound.x = tile_rect[i].x - 5;
                bound.y = tile_rect[i].y - 5;
                bound.w = tile_rect[i].w + 10;
                bound.h = tile_rect[i].h + 10;
                set_render_draw_color(screen.renderer, colors.white);
                SDL_RenderFillRect(screen.renderer, &bound);
            }

            if (tile_selected == i)
            {
                SDL_Rect bound;
                bound.x = tile_rect[i].x - 5;
                bound.y = tile_rect[i].y - 5;
                bound.w = tile_rect[i].w + 10;
                bound.h = tile_rect[i].h + 10;
                set_render_draw_color(screen.renderer, colors.red);
                SDL_RenderFillRect(screen.renderer, &bound);
            }

            SDL_RenderCopy(
                screen.renderer,
                sprite->texture,
                &sprite->rect,
                &tile_rect[i]
            );
        }
        
        SDL_RenderPresent(screen.renderer);
    }


    delete_spritesheet(&spritesheet);
    delete_tileset(&tileset);


    delete_map(&map);

    SDL_DestroyRenderer(screen.renderer);
    SDL_DestroyWindow(screen.window);
    SDL_Quit();

    screen.renderer = NULL;
    screen.window = NULL;
    return 0;
}