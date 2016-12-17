#include "SDL2/SDL.h"
#include "../frontend.h"
#include "../../window.h"
#include "../../display.h"
#include "../../input.h"
#include "../../gbe.h"

#define WINDOW_HEIGHT 400
#define WINDOW_WIDTH 500

SDL_Window* window = NULL;
SDL_Surface* screen = NULL;
SDL_Texture* texture = NULL;
SDL_Renderer* renderer = NULL;

// Swap buffers
void frontend_swap_buffers() {    
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

// Return current window size
void frontend_get_window_size(windowSize *size) {
    int width = 0, height = 0;
    SDL_GetWindowSize(window, &width, &height);
    size->width = width;
    size->height = height;
}

// Get texture ready to be displayed
void displayOnWindow(uint8 *frameBuffer) {
    SDL_Surface* frame = SDL_CreateRGBSurfaceFrom(frameBuffer, DISPLAY_WIDTH, DISPLAY_HEIGHT, 32, DISPLAY_WIDTH * 4, 0, 0, 0, 0);
    if (texture == NULL) {
        texture = SDL_CreateTextureFromSurface(renderer, frame);
    } else {
        SDL_UpdateTexture(texture, NULL, frame->pixels, frame->pitch);
    }
    SDL_FreeSurface(frame);
    frontend_swap_buffers();    
}

// Get current input
void getInput(input *current_input) {
    
}

// Start SDL window 
void startDisplay() {
    SDL_Init(SDL_INIT_EVERYTHING);
    window = SDL_CreateWindow("GBE", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    screen = SDL_GetWindowSurface(window);
    renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(renderer, WINDOW_WIDTH, WINDOW_HEIGHT);
}

void stopDisplay() {
     // Remove screen, texture, render, and window
     SDL_DestroyTexture(texture);
     SDL_FreeSurface(screen);
     SDL_DestroyRenderer(renderer);
     SDL_DestroyWindow(window);
     texture = NULL;
     screen = NULL;
     renderer = NULL;
     window = NULL;
     // Quit SDL
     SDL_Quit();
}

// Run emulator from this method.
int main(int argc, char *argv[]) {
    startDisplay();
    int out = startEmulator(argc, argv);
    stopDisplay();
    return out;
}

