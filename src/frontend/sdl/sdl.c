#include "SDL2/SDL.h"
#include "../frontend.h"
#include "../../window.h"
#include "../../display.h"
#include "../../input.h"
#include "../../gbe.h"

#define WINDOW_HEIGHT 144
#define WINDOW_WIDTH 160

SDL_Window* window = NULL;
SDL_Surface* screen = NULL;
SDL_Surface* frame = NULL;

// Swap buffers
void frontend_swap_buffers() {
    SDL_UpdateWindowSurface(window);
}

void frontend_get_window_size(windowSize *size) {
    int width = 0, height = 0;
    SDL_GetWindowSize(window, &width, &height);
    size->width = width;
    size->height = height;
}

void displayOnWindow(uint8 *frameBuffer) {
    frame = SDL_CreateRGBSurfaceFrom(frameBuffer, WINDOW_WIDTH, WINDOW_HEIGHT, 24, WINDOW_WIDTH * 3, 0x000000ff, 0x0000ff00, 0x00ff0000, 0);
    SDL_BlitSurface(frame, NULL, screen, NULL);
    frontend_swap_buffers();
    frame = NULL;
}

void getInput(input *current_input) {
    
}

// Start SDL window 
void startDisplay() {
    SDL_Init(SDL_INIT_EVERYTHING);
    window = SDL_CreateWindow("GBE", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    screen = SDL_GetWindowSurface(window);
    //SDL_Delay(2000);
}

void stopDisplay() {
     // Remove screen and window
     SDL_FreeSurface(frame);
     SDL_FreeSurface(screen);     
     SDL_DestroyWindow(window);
     frame = NULL;
     screen = NULL;     
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

