#include "graphics.h"

void setup_graphics(SDL_OBJECTS* graphics_container){
    // retutns zero on success else non-zero
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("error initializing SDL: %s\n", SDL_GetError());
    }
    SDL_Window* win = SDL_CreateWindow("GAME", // creates a window
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       640, 320, 0);

    // triggers the program that controls your graphics hardware and sets flags
    Uint32 render_flags = SDL_RENDERER_ACCELERATED;

    // creates a renderer to render our images
    SDL_Renderer* rend = SDL_CreateRenderer(win, -1, render_flags);

    // creates a surface to load an image into the main memory
    SDL_Surface* screen_surface;
    screen_surface = SDL_GetWindowSurface(win);

    for (int i=0; i<SCREEN_WIDTH; i++){
        for (int j=0; j<SCREEN_LENGTH; j++){
            graphics_container->rects[i+j*SCREEN_WIDTH].x = i*10;
            graphics_container->rects[i+j*SCREEN_WIDTH].y = j*10;
            graphics_container->rects[i+j*SCREEN_WIDTH].w = 10;
            graphics_container->rects[i+j*SCREEN_WIDTH].h = 10;
        }
    }

    graphics_container->win = win;
    graphics_container->rend = rend;
    graphics_container->screen_surface = screen_surface;
}

void update_graphics(SDL_OBJECTS* graphics_container){

    // We clear what we draw before
    SDL_SetRenderDrawColor(graphics_container->rend, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(graphics_container->rend);

    for (int i=0; i<SCREEN_WIDTH; i++){
        for (int j=0; j<SCREEN_LENGTH; j++){
            if (chip8_emu.gfx[i+j*SCREEN_WIDTH]){
                SDL_SetRenderDrawColor(graphics_container->rend, 0x00, 0x00, 0x00, 0xFF);
            }
            SDL_RenderDrawRect(graphics_container->rend, &(graphics_container->rects[i+j*SCREEN_WIDTH]));
            SDL_RenderFillRect(graphics_container->rend, &(graphics_container->rects[i+j*SCREEN_WIDTH]));
        }
    }

    // Set the color to what was before
    SDL_SetRenderDrawColor(graphics_container->rend, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderPresent(graphics_container->rend);
    // calculates to 60/4 fps
    SDL_Delay(1000 / 60);
}

void handle_key_press(){
    SDL_Event event;

    // Events mangement
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_KEYDOWN:
                // keyboard API for key pressed
                switch (event.key.keysym.scancode) {
                    case SDL_SCANCODE_1:
                        chip8_emu.key[0] = 1;
                    case SDL_SCANCODE_2:
                        chip8_emu.key[1] = 1;
                    case SDL_SCANCODE_3:
                        chip8_emu.key[2] = 1;
                    case SDL_SCANCODE_4:
                        chip8_emu.key[3] = 1;
                    case SDL_SCANCODE_Q:
                        chip8_emu.key[4] = 1;
                    case SDL_SCANCODE_W:
                        chip8_emu.key[5] = 1;
                    case SDL_SCANCODE_E:
                        chip8_emu.key[6] = 1;
                    case SDL_SCANCODE_R:
                        chip8_emu.key[7] = 1;
                    case SDL_SCANCODE_A:
                        chip8_emu.key[8] = 1;
                    case SDL_SCANCODE_S:
                        chip8_emu.key[9] = 1;
                    case SDL_SCANCODE_D:
                        chip8_emu.key[10] = 1;
                    case SDL_SCANCODE_F:
                        chip8_emu.key[11] = 1;
                    case SDL_SCANCODE_Z:
                        chip8_emu.key[12] = 1;
                    case SDL_SCANCODE_X:
                        chip8_emu.key[13] = 1;
                    case SDL_SCANCODE_C:
                        chip8_emu.key[14] = 1;
                    case SDL_SCANCODE_V:
                        chip8_emu.key[15] = 1;
                    default:
                        break;
                }
            }
    }
}
