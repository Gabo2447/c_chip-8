#include "chip8.h"
#include <stdio.h>
#include <SDL2/SDL.h>

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
#define SCALE 10

int get_key_index(SDL_Keycode key) {
    switch (key) {
        case SDLK_1: return 0x1; // 1
        case SDLK_2: return 0x2; // 2
        case SDLK_3: return 0x3; // 3
        case SDLK_4: return 0xC; // C

        case SDLK_q: return 0x4; // 4
        case SDLK_w: return 0x5; // 5
        case SDLK_e: return 0x6; // 6
        case SDLK_r: return 0xD; // D

        case SDLK_a: return 0x7; // 7
        case SDLK_s: return 0x8; // 8
        case SDLK_d: return 0x9; // 9
        case SDLK_f: return 0xE; // E

        case SDLK_z: return 0xA; // A
        case SDLK_x: return 0x0; // 0
        case SDLK_c: return 0xB; // B
        case SDLK_v: return 0xF; // F
        default: return -1;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <ROM path>\n", argv[0]);
        return 1;
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Error initializing SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "CHIP-8 Emulator",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH * SCALE, SCREEN_HEIGHT * SCALE,
        SDL_WINDOW_SHOWN
    );
    if (!window) {
        printf("Error creating SDL window: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Error creating SDL renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    chip8_init();
    if (!chip8_running()) {
        printf("Error initializing chip8!\n");
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    chip8_load(argv[1]);
    SDL_Event event;

    bool sdl_running = true;
    while (sdl_running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                sdl_running = false;
            }

            if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
                int key_idx = get_key_index(event.key.keysym.sym);
                if (key_idx != -1) {
                    keypad[key_idx] = (event.type == SDL_KEYDOWN) ? 1 : 0;
                }
            }
        }

        chip8_cycle();

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for (int y = 0; y < SCREEN_HEIGHT; y++) {
            for (int x = 0; x < SCREEN_WIDTH; x++) {
                if (gfx[(y * SCREEN_WIDTH) + x]) {
                    SDL_Rect rect = { x * SCALE, y * SCALE, SCALE, SCALE };
                    SDL_RenderFillRect(renderer, &rect);
                }
            }
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(2); // ~60 FPS
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
